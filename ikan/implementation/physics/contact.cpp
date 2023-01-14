//
//  contact.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "contact.hpp"
#include "shape_contact.hpp"
#include "world_callbacks.hpp"

namespace physics {
  
  ContactRegister Contact::registers_[(size_t)Shape::Type::TypeCount][(size_t)Shape::Type::TypeCount];
  bool Contact::initialized_ = false;
  
  void Contact::InitializeRegisters() {
    AddType(CircleContact::Create, CircleContact::Destroy, Shape::Type::Circle, Shape::Type::Circle);
    AddType(PolygonAndCircleContact::Create, PolygonAndCircleContact::Destroy, Shape::Type::Polygon, Shape::Type::Circle);
    AddType(PolygonContact::Create, PolygonContact::Destroy, Shape::Type::Polygon, Shape::Type::Polygon);
    AddType(EdgeAndCircleContact::Create, EdgeAndCircleContact::Destroy, Shape::Type::Edge, Shape::Type::Circle);
    AddType(EdgeAndPolygonContact::Create, EdgeAndPolygonContact::Destroy, Shape::Type::Edge, Shape::Type::Polygon);
    AddType(ChainAndCircleContact::Create, ChainAndCircleContact::Destroy, Shape::Type::Chain, Shape::Type::Circle);
    AddType(ChainAndPolygonContact::Create, ChainAndPolygonContact::Destroy, Shape::Type::Chain, Shape::Type::Polygon);
  }
  
  void Contact::AddType(ContactCreateFcn* createFcn, ContactDestroyFcn* destoryFcn,
                        Shape::Type type1, Shape::Type type2) {
    IK_ASSERT((0 <= (uint32_t)type1 and type1 < Shape::Type::TypeCount));
    IK_ASSERT((0 <= (uint32_t)type2 and type2 < Shape::Type::TypeCount));

    registers_[(size_t)type1][(size_t)type2].createFcn = createFcn;
    registers_[(size_t)type1][(size_t)type2].destroyFcn = destoryFcn;
    registers_[(size_t)type1][(size_t)type2].primary = true;

    if (type1 != type2) {
      registers_[(size_t)type2][(size_t)type1].createFcn = createFcn;
      registers_[(size_t)type2][(size_t)type1].destroyFcn = destoryFcn;
      registers_[(size_t)type2][(size_t)type1].primary = false;
    }
  }

  Contact* Contact::Create(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator) {
    if (initialized_ == false) {
      InitializeRegisters();
      initialized_ = true;
    }

    Shape::Type type1 = fixtureA->GetType();
    Shape::Type type2 = fixtureB->GetType();

    IK_ASSERT(0 <= (uint32_t)type1 && type1 < Shape::Type::TypeCount);
    IK_ASSERT(0 <= (uint32_t)type2 && type2 < Shape::Type::TypeCount);

    ContactCreateFcn* createFcn = registers_[(size_t)type1][(size_t)type2].createFcn;
    if (createFcn) {
      if (registers_[(size_t)type1][(size_t)type2].primary) {
        return createFcn(fixtureA, indexA, fixtureB, indexB, allocator);
      } else {
        return createFcn(fixtureB, indexB, fixtureA, indexA, allocator);
      }
    }
    else {
      return nullptr;
    }
  }

  void Contact::Destroy(Contact* contact, BlockAllocator* allocator) {
    IK_ASSERT(initialized_ == true);

    Fixture* fixtureA = contact->fixture_a_;
    Fixture* fixtureB = contact->fixture_b_;

    if (contact->manifold_.point_count > 0 &&
        fixtureA->IsSensor() == false &&
        fixtureB->IsSensor() == false) {
      fixtureA->GetBody()->SetAwake(true);
      fixtureB->GetBody()->SetAwake(true);
    }

    Shape::Type typeA = fixtureA->GetType();
    Shape::Type typeB = fixtureB->GetType();

    IK_ASSERT(0 <= (uint32_t)typeA && typeA < Shape::Type::TypeCount);
    IK_ASSERT(0 <= (uint32_t)typeB && typeB < Shape::Type::TypeCount);

    ContactDestroyFcn* destroyFcn = registers_[(size_t)typeA][(size_t)typeB].destroyFcn;
    destroyFcn(contact, allocator);
  }

  Contact::Contact(Fixture* fA, int32_t indexA, Fixture* fB, int32_t indexB) {
    flags_ = EnabledFlag;

    fixture_a_ = fA;
    fixture_b_ = fB;

    index_a_ = indexA;
    index_b_ = indexB;

    manifold_.point_count = 0;

    prev_ = nullptr;
    next_ = nullptr;

    node_a_.contact = nullptr;
    node_a_.prev = nullptr;
    node_a_.next = nullptr;
    node_a_.other = nullptr;

    node_b_.contact = nullptr;
    node_b_.prev = nullptr;
    node_b_.next = nullptr;
    node_b_.other = nullptr;

    toi_count_ = 0;

    friction_ = MixFriction(fixture_a_->m_friction, fixture_b_->m_friction);
    restitution_ = MixRestitution(fixture_a_->m_restitution, fixture_b_->m_restitution);
    restitution_threshold_ = MixRestitutionThreshold(fixture_a_->m_restitutionThreshold, fixture_b_->m_restitutionThreshold);

    tangent_speed_ = 0.0f;
  }

  // Update the contact manifold and touching status.
  // Note: do not assume the fixture AABBs are overlapping or are valid.
  void Contact::Update(ContactListener* listener) {
    Manifold oldManifold = manifold_;

    // Re-enable this contact.
    flags_ |= EnabledFlag;

    bool touching = false;
    bool wasTouching = (flags_ & TouchingFlag) == TouchingFlag;

    bool sensorA = fixture_a_->IsSensor();
    bool sensorB = fixture_b_->IsSensor();
    bool sensor = sensorA || sensorB;

    Body* bodyA = fixture_a_->GetBody();
    Body* bodyB = fixture_b_->GetBody();
    const Transform& xfA = bodyA->GetTransform();
    const Transform& xfB = bodyB->GetTransform();

    // Is this contact a sensor?
    if (sensor) {
      const Shape* shapeA = fixture_a_->GetShape();
      const Shape* shapeB = fixture_b_->GetShape();
      touching = TestOverlap(shapeA, index_a_, shapeB, index_b_, xfA, xfB);

      // Sensors don't generate manifolds.
      manifold_.point_count = 0;
    }
    else {
      Evaluate(&manifold_, xfA, xfB);
      touching = manifold_.point_count > 0;

      // Match old contact ids to new contact ids and copy the
      // stored impulses to warm start the solver.
      for (int32_t i = 0; i < manifold_.point_count; ++i) {
        ManifoldPoint* mp2 = manifold_.points + i;
        mp2->normal_impulse = 0.0f;
        mp2->tangent_impulse = 0.0f;
        ContactID id2 = mp2->id;

        for (int32_t j = 0; j < oldManifold.point_count; ++j) {
          ManifoldPoint* mp1 = oldManifold.points + j;

          if (mp1->id.key == id2.key) {
            mp2->normal_impulse = mp1->normal_impulse;
            mp2->tangent_impulse = mp1->tangent_impulse;
            break;
          }
        }
      }

      if (touching != wasTouching) {
        bodyA->SetAwake(true);
        bodyB->SetAwake(true);
      }
    }

    if (touching) {
      flags_ |= TouchingFlag;
    }
    else {
      flags_ &= ~TouchingFlag;
    }

    if (wasTouching == false && touching == true && listener) {
      listener->BeginContact(this);
    }

    if (wasTouching == true && touching == false && listener) {
      listener->EndContact(this);
    }

    if (sensor == false && touching && listener) {
      listener->PreSolve(this, &oldManifold);
    }
  }

  Manifold* Contact::GetManifold() {
    return &manifold_;
  }
  
  const Manifold* Contact::GetManifold() const {
    return &manifold_;
  }
  
  void Contact::GetWorldManifold(WorldManifold* worldManifold) const {
    const Body* bodyA = fixture_a_->GetBody();
    const Body* bodyB = fixture_b_->GetBody();
    const Shape* shapeA = fixture_a_->GetShape();
    const Shape* shapeB = fixture_b_->GetShape();
    
    worldManifold->Initialize(&manifold_, bodyA->GetTransform(), shapeA->radius_, bodyB->GetTransform(), shapeB->radius_);
  }
  
  void Contact::SetEnabled(bool flag) {
    if (flag) {
      flags_ |= EnabledFlag;
    }
    else {
      flags_ &= ~EnabledFlag;
    }
  }
  
  bool Contact::IsEnabled() const {
    return (flags_ & EnabledFlag) == EnabledFlag;
  }
  
  bool Contact::IsTouching() const {
    return (flags_ & TouchingFlag) == TouchingFlag;
  }
  
  Contact* Contact::GetNext() {
    return next_;
  }
  
  const Contact* Contact::GetNext() const {
    return next_;
  }
  
  Fixture* Contact::GetFixtureA() {
    return fixture_a_;
  }
  
  const Fixture* Contact::GetFixtureA() const {
    return fixture_a_;
  }
  
  Fixture* Contact::GetFixtureB() {
    return fixture_b_;
  }
  
  int32_t Contact::GetChildIndexA() const {
    return index_a_;
  }
  
  const Fixture* Contact::GetFixtureB() const {
    return fixture_b_;
  }
  
  int32_t Contact::GetChildIndexB() const {
    return index_b_;
  }
  
  void Contact::FlagForFiltering() {
    flags_ |= FilterFlag;
  }
  
  void Contact::SetFriction(float friction) {
    friction_ = friction;
  }
  
  float Contact::GetFriction() const {
    return friction_;
  }
  
  void Contact::ResetFriction() {
    friction_ = MixFriction(fixture_a_->m_friction, fixture_b_->m_friction);
  }
  
  void Contact::SetRestitution(float restitution) {
    restitution_ = restitution;
  }
  
  float Contact::GetRestitution() const {
    return restitution_;
  }
  
  void Contact::ResetRestitution() {
    restitution_ = MixRestitution(fixture_a_->m_restitution, fixture_b_->m_restitution);
  }
  
  void Contact::SetRestitutionThreshold(float threshold) {
    restitution_threshold_ = threshold;
  }
  
  float Contact::GetRestitutionThreshold() const {
    return restitution_threshold_;
  }
  
  void Contact::ResetRestitutionThreshold() {
    restitution_threshold_ = MixRestitutionThreshold(fixture_a_->m_restitutionThreshold, fixture_b_->m_restitutionThreshold);
  }
  
  void Contact::SetTangentSpeed(float speed) {
    tangent_speed_ = speed;
  }
  
  float Contact::GetTangentSpeed() const {
    return tangent_speed_;
  }
  
}

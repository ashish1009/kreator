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
  
  ContactRegister Contact::s_registers[(size_t)Shape::Type::TypeCount][(size_t)Shape::Type::TypeCount];
  bool Contact::s_initialized = false;
  
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

    s_registers[(size_t)type1][(size_t)type2].createFcn = createFcn;
    s_registers[(size_t)type1][(size_t)type2].destroyFcn = destoryFcn;
    s_registers[(size_t)type1][(size_t)type2].primary = true;

    if (type1 != type2) {
      s_registers[(size_t)type2][(size_t)type1].createFcn = createFcn;
      s_registers[(size_t)type2][(size_t)type1].destroyFcn = destoryFcn;
      s_registers[(size_t)type2][(size_t)type1].primary = false;
    }
  }

  Contact* Contact::Create(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator) {
    if (s_initialized == false) {
      InitializeRegisters();
      s_initialized = true;
    }

    Shape::Type type1 = fixtureA->GetType();
    Shape::Type type2 = fixtureB->GetType();

    IK_ASSERT(0 <= (uint32_t)type1 && type1 < Shape::Type::TypeCount);
    IK_ASSERT(0 <= (uint32_t)type2 && type2 < Shape::Type::TypeCount);

    ContactCreateFcn* createFcn = s_registers[(size_t)type1][(size_t)type2].createFcn;
    if (createFcn) {
      if (s_registers[(size_t)type1][(size_t)type2].primary) {
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
    IK_ASSERT(s_initialized == true);

    Fixture* fixtureA = contact->m_fixtureA;
    Fixture* fixtureB = contact->m_fixtureB;

    if (contact->m_manifold.point_count > 0 &&
        fixtureA->IsSensor() == false &&
        fixtureB->IsSensor() == false) {
      fixtureA->GetBody()->SetAwake(true);
      fixtureB->GetBody()->SetAwake(true);
    }

    Shape::Type typeA = fixtureA->GetType();
    Shape::Type typeB = fixtureB->GetType();

    IK_ASSERT(0 <= (uint32_t)typeA && typeA < Shape::Type::TypeCount);
    IK_ASSERT(0 <= (uint32_t)typeB && typeB < Shape::Type::TypeCount);

    ContactDestroyFcn* destroyFcn = s_registers[(size_t)typeA][(size_t)typeB].destroyFcn;
    destroyFcn(contact, allocator);
  }

  Contact::Contact(Fixture* fA, int32_t indexA, Fixture* fB, int32_t indexB) {
    m_flags = EnabledFlag;

    m_fixtureA = fA;
    m_fixtureB = fB;

    m_indexA = indexA;
    m_indexB = indexB;

    m_manifold.point_count = 0;

    m_prev = nullptr;
    m_next = nullptr;

    m_nodeA.contact = nullptr;
    m_nodeA.prev = nullptr;
    m_nodeA.next = nullptr;
    m_nodeA.other = nullptr;

    m_nodeB.contact = nullptr;
    m_nodeB.prev = nullptr;
    m_nodeB.next = nullptr;
    m_nodeB.other = nullptr;

    m_toiCount = 0;

    m_friction = MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
    m_restitution = MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);
    m_restitutionThreshold = MixRestitutionThreshold(m_fixtureA->m_restitutionThreshold, m_fixtureB->m_restitutionThreshold);

    m_tangentSpeed = 0.0f;
  }

  // Update the contact manifold and touching status.
  // Note: do not assume the fixture AABBs are overlapping or are valid.
  void Contact::Update(ContactListener* listener) {
    Manifold oldManifold = m_manifold;

    // Re-enable this contact.
    m_flags |= EnabledFlag;

    bool touching = false;
    bool wasTouching = (m_flags & TouchingFlag) == TouchingFlag;

    bool sensorA = m_fixtureA->IsSensor();
    bool sensorB = m_fixtureB->IsSensor();
    bool sensor = sensorA || sensorB;

    Body* bodyA = m_fixtureA->GetBody();
    Body* bodyB = m_fixtureB->GetBody();
    const Transform& xfA = bodyA->GetTransform();
    const Transform& xfB = bodyB->GetTransform();

    // Is this contact a sensor?
    if (sensor) {
      const Shape* shapeA = m_fixtureA->GetShape();
      const Shape* shapeB = m_fixtureB->GetShape();
      touching = TestOverlap(shapeA, m_indexA, shapeB, m_indexB, xfA, xfB);

      // Sensors don't generate manifolds.
      m_manifold.point_count = 0;
    }
    else {
      Evaluate(&m_manifold, xfA, xfB);
      touching = m_manifold.point_count > 0;

      // Match old contact ids to new contact ids and copy the
      // stored impulses to warm start the solver.
      for (int32_t i = 0; i < m_manifold.point_count; ++i) {
        ManifoldPoint* mp2 = m_manifold.points + i;
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
      m_flags |= TouchingFlag;
    }
    else {
      m_flags &= ~TouchingFlag;
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

  
}

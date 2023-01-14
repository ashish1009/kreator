//
//  fixture.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "fixture.hpp"
#include "block_allocator.hpp"
#include "broad_phase.hpp"
#include "circle_shape.hpp"
#include "edge_shape.hpp"
#include "chain_shape.hpp"
#include "polygon_shape.hpp"
#include "contact.hpp"
#include "world.hpp"

namespace physics {
  
  Fixture::Fixture() {
    body_ = nullptr;
    next_ = nullptr;
    proxies_ = nullptr;
    proxy_count_ = 0;
    shape_ = nullptr;
    density_ = 0.0f;
  }
  
  void Fixture::Create(BlockAllocator* allocator, Body* body, const FixtureDef* def) {
    user_data_ = def->userData;
    friction_ = def->friction;
    restitution_ = def->restitution;
    restitution_threshold_ = def->restitution_threshold;
    
    body_ = body;
    next_ = nullptr;
    
    filter_ = def->filter;
    
    is_sensor_ = def->is_sensor;
    
    shape_ = def->shape->Clone(allocator);
    
    // Reserve proxy space
    int32_t childCount = shape_->GetChildCount();
    proxies_ = (FixtureProxy*)allocator->Allocate(childCount * sizeof(FixtureProxy));
    for (int32_t i = 0; i < childCount; ++i)
    {
      proxies_[i].fixture = nullptr;
      proxies_[i].proxy_id = BroadPhase::NullProxy;
    }
    proxy_count_ = 0;
    
    density_ = def->density;
  }
  
  void Fixture::Destroy(BlockAllocator* allocator) {
    // The proxies must be destroyed before calling this.
    IK_ASSERT(proxy_count_ == 0);
    
    // Free the proxy array.
    int32_t childCount = shape_->GetChildCount();
    allocator->Free(proxies_, childCount * sizeof(FixtureProxy));
    proxies_ = nullptr;
    
    // Free the child shape.
    switch (shape_->type_) {
      case Shape::Type::Circle: {
        CircleShape* s = (CircleShape*)shape_;
        s->~CircleShape();
        allocator->Free(s, sizeof(CircleShape));
      }
      break;
        
      case Shape::Type::Edge: {
        EdgeShape* s = (EdgeShape*)shape_;
        s->~EdgeShape();
        allocator->Free(s, sizeof(EdgeShape));
      }
      break;

      case Shape::Type::Polygon: {
        PolygonShape* s = (PolygonShape*)shape_;
        s->~PolygonShape();
        allocator->Free(s, sizeof(PolygonShape));
      }
      break;

      case Shape::Type::Chain: {
        ChainShape* s = (ChainShape*)shape_;
        s->~ChainShape();
        allocator->Free(s, sizeof(ChainShape));
      }
      break;
        
      default:
        IK_ASSERT(false); break;
    }
    
    shape_ = nullptr;
  }
  
  void Fixture::CreateProxies(BroadPhase* broadPhase, const Transform& xf) {
    IK_ASSERT(proxy_count_ == 0);

    // Create proxies in the broad-phase.
    proxy_count_ = shape_->GetChildCount();

    for (int32_t i = 0; i < proxy_count_; ++i) {
      FixtureProxy* proxy = proxies_ + i;
      shape_->ComputeAABB(&proxy->aabb, xf, i);
      proxy->proxy_id = broadPhase->CreateProxy(proxy->aabb, proxy);
      proxy->fixture = this;
      proxy->child_index = i;
    }
  }

  void Fixture::DestroyProxies(BroadPhase* broadPhase) {
    // Destroy proxies in the broad-phase.
    for (int32_t i = 0; i < proxy_count_; ++i) {
      FixtureProxy* proxy = proxies_ + i;
      broadPhase->DestroyProxy(proxy->proxy_id);
      proxy->proxy_id = BroadPhase::NullProxy;
    }

    proxy_count_ = 0;
  }

  void Fixture::Synchronize(BroadPhase* broadPhase, const Transform& transform1, const Transform& transform2) {
    if (proxy_count_ == 0) {
      return;
    }

    for (int32_t i = 0; i < proxy_count_; ++i) {
      FixtureProxy* proxy = proxies_ + i;

      // Compute an AABB that covers the swept shape (may miss some rotation effect).
      AABB aabb1, aab;
      shape_->ComputeAABB(&aabb1, transform1, proxy->child_index);
      shape_->ComputeAABB(&aab, transform2, proxy->child_index);

      proxy->aabb.Combine(aabb1, aab);

      Vec2 displacement = aab.GetCenter() - aabb1.GetCenter();

      broadPhase->MoveProxy(proxy->proxy_id, proxy->aabb, displacement);
    }
  }

  void Fixture::SetFilterData(const Filter& filter) {
    filter_ = filter;

    Refilter();
  }

  void Fixture::Refilter() {
    if (body_ == nullptr) {
      return;
    }

    // Flag associated contacts for filtering.
    ContactEdge* edge = body_->GetContactList();
    while (edge) {
      Contact* contact = edge->contact;
      Fixture* fixtureA = contact->GetFixtureA();
      Fixture* fixtureB = contact->GetFixtureB();
      if (fixtureA == this || fixtureB == this) {
        contact->FlagForFiltering();
      }

      edge = edge->next;
    }

    World* world = body_->GetWorld();

    if (world == nullptr) {
      return;
    }

    // Touch each proxy so that new pairs may be created
    BroadPhase* broadPhase = &world->contact_manager.broad_phase_;
    for (int32_t i = 0; i < proxy_count_; ++i) {
      broadPhase->TouchProxy(proxies_[i].proxy_id);
    }
  }

  void Fixture::SetSensor(bool sensor) {
    if (sensor != is_sensor_) {
      body_->SetAwake(true);
      is_sensor_ = sensor;
    }
  }
  
  Shape::Type Fixture::GetType() const {
    return shape_->GetType();
  }
  
  Shape* Fixture::GetShape() {
    return shape_;
  }
  
  const Shape* Fixture::GetShape() const {
    return shape_;
  }
  
  bool Fixture::IsSensor() const {
    return is_sensor_;
  }
  
  const Filter& Fixture::GetFilterData() const {
    return filter_;
  }
  
  FixtureUserData& Fixture::GetUserData() {
    return user_data_;
  }
  
  Body* Fixture::GetBody() {
    return body_;
  }
  
  const Body* Fixture::GetBody() const {
    return body_;
  }
  
  Fixture* Fixture::GetNext() {
    return next_;
  }
  
  const Fixture* Fixture::GetNext() const {
    return next_;
  }
  
  void Fixture::SetDensity(float density) {
    IK_ASSERT(physics::IsValid(density) && density >= 0.0f);
    density_ = density;
  }
  
  float Fixture::GetDensity() const {
    return density_;
  }
  
  float Fixture::GetFriction() const {
    return friction_;
  }
  
  void Fixture::SetFriction(float friction) {
    friction_ = friction;
  }
  
  float Fixture::GetRestitution() const {
    return restitution_;
  }
  
  void Fixture::SetRestitution(float restitution) {
    restitution_ = restitution;
  }
  
  float Fixture::GetRestitutionThreshold() const {
    return restitution_threshold_;
  }
  
  void Fixture::SetRestitutionThreshold(float threshold) {
    restitution_threshold_ = threshold;
  }
  
  bool Fixture::TestPoint(const Vec2& p) const {
    return shape_->TestPoint(body_->GetTransform(), p);
  }
  
  bool Fixture::RayCast(RayCastOutput* output, const RayCastInput& input, int32_t childIndex) const {
    return shape_->RayCast(output, input, body_->GetTransform(), childIndex);
  }
  
  void Fixture::GetMassData(MassData* massData) const {
    shape_->ComputeMass(massData, density_);
  }
  
  const AABB& Fixture::GetAABB(int32_t childIndex) const {
    IK_ASSERT(0 <= childIndex && childIndex < proxy_count_);
    return proxies_[childIndex].aabb;
  }

}

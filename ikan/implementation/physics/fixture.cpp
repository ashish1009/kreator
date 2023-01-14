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
    m_body = nullptr;
    m_next = nullptr;
    m_proxies = nullptr;
    m_proxyCount = 0;
    m_shape = nullptr;
    m_density = 0.0f;
  }
  
  void Fixture::Create(BlockAllocator* allocator, Body* body, const FixtureDef* def) {
    m_userData = def->userData;
    m_friction = def->friction;
    m_restitution = def->restitution;
    m_restitutionThreshold = def->restitutionThreshold;
    
    m_body = body;
    m_next = nullptr;
    
    m_filter = def->filter;
    
    m_isSensor = def->isSensor;
    
    m_shape = def->shape->Clone(allocator);
    
    // Reserve proxy space
    int32_t childCount = m_shape->GetChildCount();
    m_proxies = (FixtureProxy*)allocator->Allocate(childCount * sizeof(FixtureProxy));
    for (int32_t i = 0; i < childCount; ++i)
    {
      m_proxies[i].fixture = nullptr;
      m_proxies[i].proxyId = BroadPhase::NullProxy;
    }
    m_proxyCount = 0;
    
    m_density = def->density;
  }
  
  void Fixture::Destroy(BlockAllocator* allocator) {
    // The proxies must be destroyed before calling this.
    IK_ASSERT(m_proxyCount == 0);
    
    // Free the proxy array.
    int32_t childCount = m_shape->GetChildCount();
    allocator->Free(m_proxies, childCount * sizeof(FixtureProxy));
    m_proxies = nullptr;
    
    // Free the child shape.
    switch (m_shape->type_) {
      case Shape::Type::Circle: {
        CircleShape* s = (CircleShape*)m_shape;
        s->~CircleShape();
        allocator->Free(s, sizeof(CircleShape));
      }
      break;
        
      case Shape::Type::Edge: {
        EdgeShape* s = (EdgeShape*)m_shape;
        s->~EdgeShape();
        allocator->Free(s, sizeof(EdgeShape));
      }
      break;

      case Shape::Type::Polygon: {
        PolygonShape* s = (PolygonShape*)m_shape;
        s->~PolygonShape();
        allocator->Free(s, sizeof(PolygonShape));
      }
      break;

      case Shape::Type::Chain: {
        ChainShape* s = (ChainShape*)m_shape;
        s->~ChainShape();
        allocator->Free(s, sizeof(ChainShape));
      }
      break;
        
      default:
        IK_ASSERT(false); break;
    }
    
    m_shape = nullptr;
  }
  
  void Fixture::CreateProxies(BroadPhase* broadPhase, const Transform& xf) {
    IK_ASSERT(m_proxyCount == 0);

    // Create proxies in the broad-phase.
    m_proxyCount = m_shape->GetChildCount();

    for (int32_t i = 0; i < m_proxyCount; ++i) {
      FixtureProxy* proxy = m_proxies + i;
      m_shape->ComputeAABB(&proxy->aabb, xf, i);
      proxy->proxyId = broadPhase->CreateProxy(proxy->aabb, proxy);
      proxy->fixture = this;
      proxy->childIndex = i;
    }
  }

  void Fixture::DestroyProxies(BroadPhase* broadPhase) {
    // Destroy proxies in the broad-phase.
    for (int32_t i = 0; i < m_proxyCount; ++i) {
      FixtureProxy* proxy = m_proxies + i;
      broadPhase->DestroyProxy(proxy->proxyId);
      proxy->proxyId = BroadPhase::NullProxy;
    }

    m_proxyCount = 0;
  }

  void Fixture::Synchronize(BroadPhase* broadPhase, const Transform& transform1, const Transform& transform2) {
    if (m_proxyCount == 0) {
      return;
    }

    for (int32_t i = 0; i < m_proxyCount; ++i) {
      FixtureProxy* proxy = m_proxies + i;

      // Compute an AABB that covers the swept shape (may miss some rotation effect).
      AABB aabb1, aab;
      m_shape->ComputeAABB(&aabb1, transform1, proxy->childIndex);
      m_shape->ComputeAABB(&aab, transform2, proxy->childIndex);

      proxy->aabb.Combine(aabb1, aab);

      Vec2 displacement = aab.GetCenter() - aabb1.GetCenter();

      broadPhase->MoveProxy(proxy->proxyId, proxy->aabb, displacement);
    }
  }

  void Fixture::SetFilterData(const Filter& filter) {
    m_filter = filter;

    Refilter();
  }

  void Fixture::Refilter() {
    if (m_body == nullptr) {
      return;
    }

    // Flag associated contacts for filtering.
    ContactEdge* edge = m_body->GetContactList();
    while (edge) {
      Contact* contact = edge->contact;
      Fixture* fixtureA = contact->GetFixtureA();
      Fixture* fixtureB = contact->GetFixtureB();
      if (fixtureA == this || fixtureB == this) {
        contact->FlagForFiltering();
      }

      edge = edge->next;
    }

    World* world = m_body->GetWorld();

    if (world == nullptr) {
      return;
    }

    // Touch each proxy so that new pairs may be created
    BroadPhase* broadPhase = &world->contact_manager.broad_phase_;
    for (int32_t i = 0; i < m_proxyCount; ++i) {
      broadPhase->TouchProxy(m_proxies[i].proxyId);
    }
  }

//  void Fixture::SetSensor(bool sensor) {
//    if (sensor != m_isSensor) {
//      m_body->SetAwake(true);
//      m_isSensor = sensor;
//    }
//  }
//
//  void Fixture::Dump(int32_t bodyIndex) {
//    Dump("    FixtureDef fd;\n");
//    Dump("    fd.friction = %.9g;\n", m_friction);
//    Dump("    fd.restitution = %.9g;\n", m_restitution);
//    Dump("    fd.restitutionThreshold = %.9g;\n", m_restitutionThreshold);
//    Dump("    fd.density = %.9g;\n", m_density);
//    Dump("    fd.isSensor = bool(%d);\n", m_isSensor);
//    Dump("    fd.filter.categoryBits = uint16(%d);\n", m_filter.categoryBits);
//    Dump("    fd.filter.maskBits = uint16(%d);\n", m_filter.maskBits);
//    Dump("    fd.filter.groupIndex = int16(%d);\n", m_filter.groupIndex);
//
//    switch (m_shape->m_type) {
//      case Shape::e_circle: {
//        CircleShape* s = (CircleShape*)m_shape;
//        Dump("    CircleShape shape;\n");
//        Dump("    shape.m_radius = %.9g;\n", s->m_radius);
//        Dump("    shape.m_p.Set(%.9g, %.9g);\n", s->m_p.x, s->m_p.y);
//      }
//        break;
//
//      case Shape::e_edge: {
//        EdgeShape* s = (EdgeShape*)m_shape;
//        Dump("    EdgeShape shape;\n");
//        Dump("    shape.m_radius = %.9g;\n", s->m_radius);
//        Dump("    shape.m_vertex0.Set(%.9g, %.9g);\n", s->m_vertex0.x, s->m_vertex0.y);
//        Dump("    shape.m_vertex1.Set(%.9g, %.9g);\n", s->m_vertex1.x, s->m_vertex1.y);
//        Dump("    shape.m_vertex2.Set(%.9g, %.9g);\n", s->m_vertex2.x, s->m_vertex2.y);
//        Dump("    shape.m_vertex3.Set(%.9g, %.9g);\n", s->m_vertex3.x, s->m_vertex3.y);
//        Dump("    shape.m_oneSided = bool(%d);\n", s->m_oneSided);
//      }
//        break;
//
//      case Shape::e_polygon: {
//        PolygonShape* s = (PolygonShape*)m_shape;
//        Dump("    PolygonShape shape;\n");
//        Dump("    Vec2 vs[%d];\n", _maxPolygonVertices);
//        for (int32_t i = 0; i < s->m_count; ++i) {
//          Dump("    vs[%d].Set(%.9g, %.9g);\n", i, s->m_vertices[i].x, s->m_vertices[i].y);
//        }
//        Dump("    shape.Set(vs, %d);\n", s->m_count);
//      }
//        break;
//
//      case Shape::e_chain: {
//        ChainShape* s = (ChainShape*)m_shape;
//        Dump("    ChainShape shape;\n");
//        Dump("    Vec2 vs[%d];\n", s->m_count);
//        for (int32_t i = 0; i < s->m_count; ++i)
//        {
//          Dump("    vs[%d].Set(%.9g, %.9g);\n", i, s->m_vertices[i].x, s->m_vertices[i].y);
//        }
//        Dump("    shape.CreateChain(vs, %d);\n", s->m_count);
//        Dump("    shape.m_prevVertex.Set(%.9g, %.9g);\n", s->m_prevVertex.x, s->m_prevVertex.y);
//        Dump("    shape.m_nextVertex.Set(%.9g, %.9g);\n", s->m_nextVertex.x, s->m_nextVertex.y);
//      }
//        break;
//
//      default:
//        return;
//    }
//
//    Dump("\n");
//    Dump("    fd.shape = &shape;\n");
//    Dump("\n");
//    Dump("    bodies[%d]->CreateFixture(&fd);\n", bodyIndex);
//  }

  Shape::Type Fixture::GetType() const {
    return m_shape->GetType();
  }
  
  Shape* Fixture::GetShape() {
    return m_shape;
  }
  
  const Shape* Fixture::GetShape() const {
    return m_shape;
  }
  
  bool Fixture::IsSensor() const {
    return m_isSensor;
  }
  
  const Filter& Fixture::GetFilterData() const {
    return m_filter;
  }
  
  FixtureUserData& Fixture::GetUserData() {
    return m_userData;
  }
  
  Body* Fixture::GetBody() {
    return m_body;
  }
  
  const Body* Fixture::GetBody() const {
    return m_body;
  }
  
  Fixture* Fixture::GetNext() {
    return m_next;
  }
  
  const Fixture* Fixture::GetNext() const {
    return m_next;
  }
  
  void Fixture::SetDensity(float density) {
    IK_ASSERT(physics::IsValid(density) && density >= 0.0f);
    m_density = density;
  }
  
  float Fixture::GetDensity() const {
    return m_density;
  }
  
  float Fixture::GetFriction() const {
    return m_friction;
  }
  
  void Fixture::SetFriction(float friction) {
    m_friction = friction;
  }
  
  float Fixture::GetRestitution() const {
    return m_restitution;
  }
  
  void Fixture::SetRestitution(float restitution) {
    m_restitution = restitution;
  }
  
  float Fixture::GetRestitutionThreshold() const {
    return m_restitutionThreshold;
  }
  
  void Fixture::SetRestitutionThreshold(float threshold) {
    m_restitutionThreshold = threshold;
  }
  
  bool Fixture::TestPoint(const Vec2& p) const {
    return m_shape->TestPoint(m_body->GetTransform(), p);
  }
  
  bool Fixture::RayCast(RayCastOutput* output, const RayCastInput& input, int32_t childIndex) const {
    return m_shape->RayCast(output, input, m_body->GetTransform(), childIndex);
  }
  
  void Fixture::GetMassData(MassData* massData) const {
    m_shape->ComputeMass(massData, m_density);
  }
  
  const AABB& Fixture::GetAABB(int32_t childIndex) const {
    IK_ASSERT(0 <= childIndex && childIndex < m_proxyCount);
    return m_proxies[childIndex].aabb;
  }

}

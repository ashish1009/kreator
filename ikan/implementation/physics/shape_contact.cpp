//
//  shape_contact.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "shape_contact.hpp"
#include "block_allocator.hpp"
#include "circle_shape.hpp"

namespace physics {
  
  Contact* CircleContact::Create(Fixture* fixtureA, int32_t, Fixture* fixtureB, int32_t, BlockAllocator* allocator) {
    void* mem = allocator->Allocate(sizeof(CircleContact));
    return new (mem) CircleContact(fixtureA, fixtureB);
  }
  
  void CircleContact::Destroy(Contact* contact, BlockAllocator* allocator) {
    ((CircleContact*)contact)->~CircleContact();
    allocator->Free(contact, sizeof(CircleContact));
  }
  
  CircleContact::CircleContact(Fixture* fixtureA, Fixture* fixtureB)
  : Contact(fixtureA, 0, fixtureB, 0) {
    IK_ASSERT(m_fixtureA->GetType() == Shape::Type::Circle);
    IK_ASSERT(m_fixtureB->GetType() == Shape::Type::Circle);
  }
  
  void CircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) {
    CollideCircles(manifold,
                   (CircleShape*)m_fixtureA->GetShape(), xfA,
                   (CircleShape*)m_fixtureB->GetShape(), xfB);
  }

  Contact* PolygonAndCircleContact::Create(Fixture* fixtureA, int32_t, Fixture* fixtureB, int32_t, BlockAllocator* allocator) {
    void* mem = allocator->Allocate(sizeof(PolygonAndCircleContact));
    return new (mem) PolygonAndCircleContact(fixtureA, fixtureB);
  }
  
  void PolygonAndCircleContact::Destroy(Contact* contact, BlockAllocator* allocator) {
    ((PolygonAndCircleContact*)contact)->~PolygonAndCircleContact();
    allocator->Free(contact, sizeof(PolygonAndCircleContact));
  }
  
  PolygonAndCircleContact::PolygonAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
  : Contact(fixtureA, 0, fixtureB, 0) {
    IK_ASSERT(m_fixtureA->GetType() == Shape::Type::Polygon);
    IK_ASSERT(m_fixtureB->GetType() == Shape::Type::Circle);
  }
  
  void PolygonAndCircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) {
    CollidePolygonAndCircle( manifold,
                            (PolygonShape*)m_fixtureA->GetShape(), xfA,
                            (CircleShape*)m_fixtureB->GetShape(), xfB);
  }

}

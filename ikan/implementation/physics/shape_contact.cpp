//
//  shape_contact.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "shape_contact.hpp"
#include "block_allocator.hpp"
#include "circle_shape.hpp"
#include "chain_shape.hpp"
#include "edge_shape.hpp"

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

  Contact* PolygonContact::Create(Fixture* fixtureA, int32_t, Fixture* fixtureB, int32_t, BlockAllocator* allocator) {
    void* mem = allocator->Allocate(sizeof(PolygonContact));
    return new (mem) PolygonContact(fixtureA, fixtureB);
  }
  
  void PolygonContact::Destroy(Contact* contact, BlockAllocator* allocator) {
    ((PolygonContact*)contact)->~PolygonContact();
    allocator->Free(contact, sizeof(PolygonContact));
  }
  
  PolygonContact::PolygonContact(Fixture* fixtureA, Fixture* fixtureB)
  : Contact(fixtureA, 0, fixtureB, 0) {
    IK_ASSERT(m_fixtureA->GetType() == Shape::Type::Polygon);
    IK_ASSERT(m_fixtureB->GetType() == Shape::Type::Polygon);
  }
  
  void PolygonContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) {
    CollidePolygons(  manifold,
                      (PolygonShape*)m_fixtureA->GetShape(), xfA,
                      (PolygonShape*)m_fixtureB->GetShape(), xfB);
  }

  
  Contact* EdgeAndCircleContact::Create(Fixture* fixtureA, int32_t, Fixture* fixtureB, int32_t, BlockAllocator* allocator) {
    void* mem = allocator->Allocate(sizeof(EdgeAndCircleContact));
    return new (mem) EdgeAndCircleContact(fixtureA, fixtureB);
  }
  
  void EdgeAndCircleContact::Destroy(Contact* contact, BlockAllocator* allocator) {
    ((EdgeAndCircleContact*)contact)->~EdgeAndCircleContact();
    allocator->Free(contact, sizeof(EdgeAndCircleContact));
  }
  
  EdgeAndCircleContact::EdgeAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
  : Contact(fixtureA, 0, fixtureB, 0) {
    IK_ASSERT(m_fixtureA->GetType() == Shape::Type::Edge);
    IK_ASSERT(m_fixtureB->GetType() == Shape::Type::Circle);
  }
  
  void EdgeAndCircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) {
    CollideEdgeAndCircle(  manifold,
                           (EdgeShape*)m_fixtureA->GetShape(), xfA,
                           (CircleShape*)m_fixtureB->GetShape(), xfB);
  }


  Contact* EdgeAndPolygonContact::Create(Fixture* fixtureA, int32_t, Fixture* fixtureB, int32_t, BlockAllocator* allocator) {
    void* mem = allocator->Allocate(sizeof(EdgeAndPolygonContact));
    return new (mem) EdgeAndPolygonContact(fixtureA, fixtureB);
  }
  
  void EdgeAndPolygonContact::Destroy(Contact* contact, BlockAllocator* allocator) {
    ((EdgeAndPolygonContact*)contact)->~EdgeAndPolygonContact();
    allocator->Free(contact, sizeof(EdgeAndPolygonContact));
  }
  
  EdgeAndPolygonContact::EdgeAndPolygonContact(Fixture* fixtureA, Fixture* fixtureB)
  : Contact(fixtureA, 0, fixtureB, 0) {
    IK_ASSERT(m_fixtureA->GetType() == Shape::Type::Edge);
    IK_ASSERT(m_fixtureB->GetType() == Shape::Type::Polygon);
  }
  
  void EdgeAndPolygonContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) {
    CollideEdgeAndPolygon(  manifold,
                            (EdgeShape*)m_fixtureA->GetShape(), xfA,
                            (PolygonShape*)m_fixtureB->GetShape(), xfB);
  }

  Contact* ChainAndCircleContact::Create(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator) {
    void* mem = allocator->Allocate(sizeof(ChainAndCircleContact));
    return new (mem) ChainAndCircleContact(fixtureA, indexA, fixtureB, indexB);
  }
  
  void ChainAndCircleContact::Destroy(Contact* contact, BlockAllocator* allocator) {
    ((ChainAndCircleContact*)contact)->~ChainAndCircleContact();
    allocator->Free(contact, sizeof(ChainAndCircleContact));
  }
  
  ChainAndCircleContact::ChainAndCircleContact(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB)
  : Contact(fixtureA, indexA, fixtureB, indexB) {
    IK_ASSERT(m_fixtureA->GetType() == Shape::Type::Chain);
    IK_ASSERT(m_fixtureB->GetType() == Shape::Type::Circle);
  }
  
  void ChainAndCircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) {
    ChainShape* chain = (ChainShape*)m_fixtureA->GetShape();
    EdgeShape edge;
    chain->GetChildEdge(&edge, m_indexA);
    CollideEdgeAndCircle(  manifold, &edge, xfA,
                           (CircleShape*)m_fixtureB->GetShape(), xfB);
  }

  Contact* ChainAndPolygonContact::Create(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator) {
    void* mem = allocator->Allocate(sizeof(ChainAndPolygonContact));
    return new (mem) ChainAndPolygonContact(fixtureA, indexA, fixtureB, indexB);
  }
  
  void ChainAndPolygonContact::Destroy(Contact* contact, BlockAllocator* allocator) {
    ((ChainAndPolygonContact*)contact)->~ChainAndPolygonContact();
    allocator->Free(contact, sizeof(ChainAndPolygonContact));
  }
  
  ChainAndPolygonContact::ChainAndPolygonContact(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB)
  : Contact(fixtureA, indexA, fixtureB, indexB) {
    IK_ASSERT(m_fixtureA->GetType() == Shape::Type::Chain);
    IK_ASSERT(m_fixtureB->GetType() == Shape::Type::Polygon);
  }
  
  void ChainAndPolygonContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) {
    ChainShape* chain = (ChainShape*)m_fixtureA->GetShape();
    EdgeShape edge;
    chain->GetChildEdge(&edge, m_indexA);
    CollideEdgeAndPolygon(  manifold, &edge, xfA,
                            (PolygonShape*)m_fixtureB->GetShape(), xfB);
  }

}

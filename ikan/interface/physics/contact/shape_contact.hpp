//
//  shape_contact.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "contact.hpp"

namespace physics {
  
  class BlockAllocator;
  
  class CircleContact : public Contact {
  public:
    static Contact* Create(Fixture* fixtureA, int32_t indexA,
                           Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);
    
    CircleContact(Fixture* fixtureA, Fixture* fixtureB);
    ~CircleContact() {}
    
    void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
  };
  
  class PolygonAndCircleContact : public Contact {
  public:
    static Contact* Create(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);
    
    PolygonAndCircleContact(Fixture* fixtureA, Fixture* fixtureB);
    ~PolygonAndCircleContact() {}
    
    void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
  };

  class PolygonContact : public Contact {
  public:
    static Contact* Create(  Fixture* fixtureA, int32_t indexA,
                             Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);
    
    PolygonContact(Fixture* fixtureA, Fixture* fixtureB);
    ~PolygonContact() {}
    
    void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
  };

  class EdgeAndCircleContact : public Contact {
  public:
    static Contact* Create(  Fixture* fixtureA, int32_t indexA,
                             Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);
    
    EdgeAndCircleContact(Fixture* fixtureA, Fixture* fixtureB);
    ~EdgeAndCircleContact() {}
    
    void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
  };
  
  class EdgeAndPolygonContact : public Contact {
  public:
    static Contact* Create(  Fixture* fixtureA, int32_t indexA,
                             Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);
    
    EdgeAndPolygonContact(Fixture* fixtureA, Fixture* fixtureB);
    ~EdgeAndPolygonContact() {}
    
    void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
  };

  class ChainAndCircleContact : public Contact {
  public:
    static Contact* Create(  Fixture* fixtureA, int32_t indexA,
                             Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);
    
    ChainAndCircleContact(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB);
    ~ChainAndCircleContact() {}
    
    void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
  };

  class ChainAndPolygonContact : public Contact {
  public:
    static Contact* Create(  Fixture* fixtureA, int32_t indexA,
                             Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);
    
    ChainAndPolygonContact(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB);
    ~ChainAndPolygonContact() {}
    
    void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
  };

}
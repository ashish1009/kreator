//
//  world.cpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#include "world.hpp"
#include "body.hpp"

namespace physics {
  
  World::World(const Vec2& gravity)
  : gravity_(gravity) {
    
  }
  
  World::~World() {
    
  }
  
  Body* World::CreateBody(const BodyDef* def) {
    Body* b = nullptr;
    return b;
  }
  
  void World::DestroyBody(Body* body) {
    
  }

  
}

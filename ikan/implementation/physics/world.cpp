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
    PHYSICS_INFO("Creating Box 2D World ...");
  }
  
  World::~World() {
    PHYSICS_WARN("Destroying Box 2D World !!!");
  }
  
  Body* World::CreateBody(const BodyDef* def) {
    PHYSICS_ASSERT(!IsLocked());
    
    void* mem = block_allocator_.Allocate(sizeof(Body));
    Body* b = new (mem)Body(def, this);
    
    // Add to world doubly linked list.
    b->prev_ = nullptr;
    b->next_ = body_list_;
    if (body_list_) {
      body_list_->prev_ = b;
    }
    body_list_ = b;
    ++body_count_;

    PHYSICS_INFO("Added a body in physics world. Total Bodies | {0}", body_count_);
    return b;
  }
  
  bool World::IsLocked() const { return locked_; }
  
}

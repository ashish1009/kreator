//
//  world.cpp
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#include "world.hpp"
#include "body.hpp"
#include "fixture.hpp"
#include "timer.hpp"

namespace physics {
  
  World::World(const Vec2& gravity) :
  gravity_(gravity),
  destruction_listener_(nullptr), debug_draw_(nullptr), body_list_(nullptr), joint_list_(nullptr),
  body_count_(0), joint_count_(0),
  warm_starting_(true), continuous_physics_(true), sub_stepping_(false),
  step_complete_(true), allow_sleep_(true),
    
  new_contacts_(false), locked_(false), clear_forces_(true),
  inv_dt_0_(0.0f) {
    contact_manager_.allocator_ = &block_allocator_;
    memset(&profile_, 0, sizeof(Profile));
  }
  
  World::~World() {
    // Some shapes allocate using b2Alloc.
    Body* b = body_list_;
    while (b) {
      Body* bNext = b->next_;
      
      Fixture* f = b->fixture_list_;
      while (f) {
        Fixture* fNext = f->next_;
        f->proxy_count_ = 0;
        f->Destroy(&block_allocator_);
        f = fNext;
      }
      
      b = bNext;
    }
  }
  
  Body* World::CreateBody(const BodyDef* def) {
    IK_ASSERT(IsLocked() == false);
    
    void* mem = block_allocator_.Allocate(sizeof(Body));
    Body* b = new (mem) Body(def, this);
    
    // Add to world doubly linked list.
    b->prev_ = nullptr;
    b->next_ = body_list_;
    if (body_list_) {
      body_list_->prev_ = b;
    }
    body_list_ = b;
    ++body_count_;
    
    return b;
  }
  
  void World::Step(float dt, int32_t velocity_iterations, int32_t position_iterations) {
    Timer step_timer;
    
    // If new fixtures were added, we need to find the new contacts.
    if (new_contacts_) {
      contact_manager_.FindNewContacts();
      new_contacts_ = false;
    }

    locked_ = true;

    TimeStep step;
    step.dt = dt;
    step.velocity_iterations  = velocity_iterations;
    step.position_iterations = position_iterations;
    if (dt > 0.0f) {
      step.inv_dt = 1.0f / dt;
    } else {
      step.inv_dt = 0.0f;
    }

    step.dt_ratio = inv_dt_0_ * dt;    
    step.warm_starting = warm_starting_;

    // Update contacts. This is where some contacts are destroyed.
    {
      Timer timer;
      contact_manager_.Collide();
      profile_.collide = timer.GetMilliseconds();
    }
    
    // Integrate velocities, solve velocity constraints, and integrate positions.
    if (step_complete_ && step.dt > 0.0f) {
      Timer timer;
      Solve(step);
      profile_.solve = timer.GetMilliseconds();
    }
  }
  
  // Find islands, integrate and solve constraints, solve position constraints
  void World::Solve(const TimeStep& step) {
  }
  
  bool World::IsLocked() const {
    return locked_;
  }
  
}

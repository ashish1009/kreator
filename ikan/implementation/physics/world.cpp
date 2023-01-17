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
#include "contact.hpp"
#include "joint.hpp"
#include "dynamics/is_land.hpp"

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
    // Some shapes allocate using Alloc.
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
    profile_.solveInit = 0.0f;
    profile_.solveVelocity = 0.0f;
    profile_.solvePosition = 0.0f;
    
    // Size the island for the worst case.
    Island island(body_count_,
                  contact_manager_.contact_count_,
                  joint_count_,
                  &stack_allocator_,
                  contact_manager_.contact_listener_);
    
    // Clear all the island flags.
    for (Body* b = body_list_; b; b = b->next_) {
      b->flags_ &= ~Body::e_islandFlag;
    }
    for (Contact* c = contact_manager_.contact_list_; c; c = c->next_) {
      c->flags_ &= ~Contact::IslandFlag;
    }
    for (Joint* j = joint_list_; j; j = j->next_) {
      j->is_land_flag_ = false;
    }
    
    // Build and simulate all awake islands.
    int32_t stackSize = body_count_;
    Body** stack = (Body**)stack_allocator_.Allocate(stackSize * sizeof(Body*));
    for (Body* seed = body_list_; seed; seed = seed->next_) {
      if (seed->flags_ & Body::e_islandFlag) {
        continue;
      }
      
      if (seed->IsAwake() == false || seed->IsEnabled() == false) {
        continue;
      }
      
      // The seed can be dynamic or kinematic.
      if (seed->GetType() == BodyType::StaticBody) {
        continue;
      }
      
      // Reset island and stack.
      island.Clear();
      int32_t stackCount = 0;
      stack[stackCount++] = seed;
      seed->flags_ |= Body::e_islandFlag;
      
      // Perform a depth first search (DFS) on the constraint graph.
      while (stackCount > 0) {
        // Grab the next body off the stack and add it to the island.
        Body* b = stack[--stackCount];
        IK_ASSERT(b->IsEnabled() == true);
        island.Add(b);
        
        // To keep islands as small as possible, we don't
        // propagate islands across static bodies.
        if (b->GetType() == BodyType::StaticBody) {
          continue;
        }
        
        // Make sure the body is awake (without resetting sleep timer).
        b->flags_ |= Body::e_awakeFlag;
        
        // Search all contacts connected to this body.
        for (ContactEdge* ce = b->contact_list_; ce; ce = ce->next) {
          Contact* contact = ce->contact;
          
          // Has this contact already been added to an island?
          if (contact->flags_ & Contact::IslandFlag) {
            continue;
          }
          
          // Is this contact solid and touching?
          if (contact->IsEnabled() == false ||
              contact->IsTouching() == false) {
            continue;
          }
          
          // Skip sensors.
          bool sensorA = contact->fixture_a_->is_sensor_;
          bool sensorB = contact->fixture_b_->is_sensor_;
          if (sensorA || sensorB) {
            continue;
          }
          
          island.Add(contact);
          contact->flags_ |= Contact::IslandFlag;
          
          Body* other = ce->other;
          
          // Was the other body already added to this island?
          if (other->flags_ & Body::e_islandFlag) {
            continue;
          }
          
          IK_ASSERT(stackCount < stackSize);
          stack[stackCount++] = other;
          other->flags_ |= Body::e_islandFlag;
        }
        
        // Search all joints connect to this body.
        for (JointEdge* je = b->joint_list_; je; je = je->next) {
          if (je->joint->is_land_flag_ == true) {
            continue;
          }
          
          Body* other = je->other;
          
          // Don't simulate joints connected to disabled bodies.
          if (other->IsEnabled() == false) {
            continue;
          }
          
          island.Add(je->joint);
          je->joint->is_land_flag_ = true;
          
          if (other->flags_ & Body::e_islandFlag) {
            continue;
          }
          
          IK_ASSERT(stackCount < stackSize);
          stack[stackCount++] = other;
          other->flags_ |= Body::e_islandFlag;
        }
      }
      
      Profile profile;
      island.Solve(&profile, step, gravity_, allow_sleep_);
      profile.solveInit += profile.solveInit;
      profile.solveVelocity += profile.solveVelocity;
      profile.solvePosition += profile.solvePosition;
      
      // Post solve cleanup.
      for (int32_t i = 0; i < island.body_count_; ++i) {
        // Allow static bodies to participate in other islands.
        Body* b = island.bodies_[i];
        if (b->GetType() == BodyType::StaticBody) {
          b->flags_ &= ~Body::e_islandFlag;
        }
      }
    }
  }
  
  bool World::IsLocked() const {
    return locked_;
  }
  
}

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
#include "time_of_impact.hpp"
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
    
    // Handle TOI events.
    if (continuous_physics_ && step.dt > 0.0f) {
      Timer timer;
      SolveTOI(step);
      profile_.solveTOI = timer.GetMilliseconds();
    }

    if (step.dt > 0.0f) {
      inv_dt_0_ = step.inv_dt;
    }
    
    if (clear_forces_) {
      ClearForces();
    }
    
    locked_ = false;
    profile_.step = step_timer.GetMilliseconds();
  }
  
  void World::ClearForces() {
    for (Body* body = body_list_; body; body = body->GetNext()) {
      body->force_.SetZero();
      body->torque_ = 0.0f;
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
    
    stack_allocator_.Free(stack);
    
    {
      Timer timer;
      // Synchronize fixtures, check for out of range bodies.
      for (Body* b = body_list_; b; b = b->GetNext()) {
        // If a body was not in an island then it did not move.
        if ((b->flags_ & Body::e_islandFlag) == 0) {
          continue;
        }
        
        if (b->GetType() == BodyType::StaticBody) {
          continue;
        }
        
        // Update fixtures (for broad-phase).
        b->SynchronizeFixtures();
      }
      
      // Look for new contacts.
      contact_manager_.FindNewContacts();
      profile_.broadphase = timer.GetMilliseconds();
    }
  }

  // Find TOI contacts and solve them.
  void World::SolveTOI(const TimeStep& step) {
    Island island(2 * MaxTOIContacts, MaxTOIContacts, 0, &stack_allocator_, contact_manager_.contact_listener_);
    
    if (step_complete_) {
      for (Body* b = body_list_; b; b = b->next_) {
        b->flags_ &= ~Body::e_islandFlag;
        b->sweep_.alpha0 = 0.0f;
      }
      
      for (Contact* c = contact_manager_.contact_list_; c; c = c->next_) {
        // Invalidate TOI
        c->flags_ &= ~(Contact::ToiFlag | Contact::IslandFlag);
        c->toi_count_ = 0;
        c->toi_ = 1.0f;
      }
    }
    
    // Find TOI events and solve them.
    for (;;) {
      // Find the first TOI.
      Contact* minContact = nullptr;
      float minAlpha = 1.0f;
      
      for (Contact* c = contact_manager_.contact_list_; c; c = c->next_) {
        // Is this contact disabled?
        if (c->IsEnabled() == false) {
          continue;
        }
        
        // Prevent excessive sub-stepping.
        if (c->toi_count_ > MaxSubSteps) {
          continue;
        }
        
        float alpha = 1.0f;
        if (c->flags_ & Contact::ToiFlag) {
          // This contact has a valid cached TOI.
          alpha = c->toi_;
        }
        else {
          Fixture* fA = c->GetFixtureA();
          Fixture* fB = c->GetFixtureB();
          
          // Is there a sensor?
          if (fA->IsSensor() || fB->IsSensor()) {
            continue;
          }
          
          Body* bA = fA->GetBody();
          Body* bB = fB->GetBody();
          
          BodyType typeA = bA->type_;
          BodyType typeB = bB->type_;
          IK_ASSERT(typeA == BodyType::DynamicBody || typeB == BodyType::DynamicBody);
          
          bool activeA = bA->IsAwake() && typeA != BodyType::StaticBody;
          bool activeB = bB->IsAwake() && typeB != BodyType::StaticBody;
          
          // Is at least one body active (awake and dynamic or kinematic)?
          if (activeA == false && activeB == false) {
            continue;
          }
          
          bool collideA = bA->IsBullet() || typeA != BodyType::DynamicBody;
          bool collideB = bB->IsBullet() || typeB != BodyType::DynamicBody;
          
          // Are these two non-bullet dynamic bodies?
          if (collideA == false && collideB == false) {
            continue;
          }
          
          // Compute the TOI for this contact.
          // Put the sweeps onto the same time interval.
          float alpha0 = bA->sweep_.alpha0;
          
          if (bA->sweep_.alpha0 < bB->sweep_.alpha0) {
            alpha0 = bB->sweep_.alpha0;
            bA->sweep_.Advance(alpha0);
          }
          else if (bB->sweep_.alpha0 < bA->sweep_.alpha0) {
            alpha0 = bA->sweep_.alpha0;
            bB->sweep_.Advance(alpha0);
          }
          
          IK_ASSERT(alpha0 < 1.0f);
          
          int32_t indexA = c->GetChildIndexA();
          int32_t indexB = c->GetChildIndexB();
          
          // Compute the time of impact in interval [0, minTOI]
          TOIInput input;
          input.proxyA.Set(fA->GetShape(), indexA);
          input.proxyB.Set(fB->GetShape(), indexB);
          input.sweepA = bA->sweep_;
          input.sweepB = bB->sweep_;
          input.tMax = 1.0f;

          TOIOutput output;
          TimeOfImpact(&output, &input);

          // Beta is the fraction of the remaining portion of the .
          float beta = output.t;
          if (output.state == TOIOutput::e_touching) {
            alpha = Min(alpha0 + (1.0f - alpha0) * beta, 1.0f);
          }
          else {
            alpha = 1.0f;
          }

          c->toi_ = alpha;
          c->flags_ |= Contact::ToiFlag;
        }

        if (alpha < minAlpha) {
          // This is the minimum TOI found so far.
          minContact = c;
          minAlpha = alpha;
        }
      }

      if (minContact == nullptr || 1.0f - 10.0f * FLT_EPSILON < minAlpha) {
        // No more TOI events. Done!
        step_complete_ = true;
        break;
      }

      // Advance the bodies to the TOI.
      Fixture* fA = minContact->GetFixtureA();
      Fixture* fB = minContact->GetFixtureB();
      Body* bA = fA->GetBody();
      Body* bB = fB->GetBody();

      Sweep backup1 = bA->sweep_;
      Sweep backup2 = bB->sweep_;

      bA->Advance(minAlpha);
      bB->Advance(minAlpha);

      // The TOI contact likely has some new contact points.
      minContact->Update(contact_manager_.contact_listener_);
      minContact->flags_ &= ~Contact::ToiFlag;
      ++minContact->toi_count_;

      // Is the contact solid?
      if (minContact->IsEnabled() == false || minContact->IsTouching() == false) {
        // Restore the sweeps.
        minContact->SetEnabled(false);
        bA->sweep_ = backup1;
        bB->sweep_ = backup2;
        bA->SynchronizeTransform();
        bB->SynchronizeTransform();
        continue;
      }

      bA->SetAwake(true);
      bB->SetAwake(true);

      // Build the island
      island.Clear();
      island.Add(bA);
      island.Add(bB);
      island.Add(minContact);

      bA->flags_ |= Body::e_islandFlag;
      bB->flags_ |= Body::e_islandFlag;
      minContact->flags_ |= Contact::IslandFlag;

      // Get contacts on bodyA and bodyB.
      Body* bodies[2] = {bA, bB};
      for (int32_t i = 0; i < 2; ++i) {
        Body* body = bodies[i];
        if (body->type_ == BodyType::DynamicBody) {
          for (ContactEdge* ce = body->contact_list_; ce; ce = ce->next) {
            if (island.body_count_ == island.body_capacity_) {
              break;
            }

            if (island.contact_count_ == island.contact_capacity_) {
              break;
            }

            Contact* contact = ce->contact;

            // Has this contact already been added to the island?
            if (contact->flags_ & Contact::IslandFlag) {
              continue;
            }

            // Only add static, kinematic, or bullet bodies.
            Body* other = ce->other;
            if (other->type_ == BodyType::DynamicBody && body->IsBullet() == false && other->IsBullet() == false) {
              continue;
            }

            // Skip sensors.
            bool sensorA = contact->fixture_a_->is_sensor_;
            bool sensorB = contact->fixture_b_->is_sensor_;
            if (sensorA || sensorB) {
              continue;
            }

            // Tentatively advance the body to the TOI.
            Sweep backup = other->sweep_;
            if ((other->flags_ & Body::e_islandFlag) == 0) {
              other->Advance(minAlpha);
            }

            // Update the contact points
            contact->Update(contact_manager_.contact_listener_);

            // Was the contact disabled by the user?
            if (contact->IsEnabled() == false) {
              other->sweep_ = backup;
              other->SynchronizeTransform();
              continue;
            }

            // Are there contact points?
            if (contact->IsTouching() == false) {
              other->sweep_ = backup;
              other->SynchronizeTransform();
              continue;
            }

            // Add the contact to the island
            contact->flags_ |= Contact::IslandFlag;
            island.Add(contact);

            // Has the other body already been added to the island?
            if (other->flags_ & Body::e_islandFlag) {
              continue;
            }

            // Add the other body to the island.
            other->flags_ |= Body::e_islandFlag;

            if (other->type_ != BodyType::StaticBody) {
              other->SetAwake(true);
            }

            island.Add(other);
          }
        }
      }

      TimeStep subStep;
      subStep.dt = (1.0f - minAlpha) * step.dt;
      subStep.inv_dt = 1.0f / subStep.dt;
      subStep.dt_ratio = 1.0f;
      subStep.position_iterations = 20;
      subStep.velocity_iterations = step.velocity_iterations;
      subStep.warm_starting = false;
      island.SolveTOI(subStep, bA->is_land_index_, bB->is_land_index_);

      // Reset island flags and synchronize broad-phase proxies.
      for (int32_t i = 0; i < island.body_count_; ++i) {
        Body* body = island.bodies_[i];
        body->flags_ &= ~Body::e_islandFlag;

        if (body->type_ != BodyType::DynamicBody) {
          continue;
        }

        body->SynchronizeFixtures();

        // Invalidate all contact TOIs on this displaced body.
        for (ContactEdge* ce = body->contact_list_; ce; ce = ce->next) {
          ce->contact->flags_ &= ~(Contact::ToiFlag | Contact::IslandFlag);
        }
      }

      // Commit fixture proxy movements to the broad-phase so that new contacts are created.
      // Also, some contacts can be destroyed.
      contact_manager_.FindNewContacts();

      if (sub_stepping_) {
        step_complete_ = false;
        break;
      }
    }
  }
  
  bool World::IsLocked() const {
    return locked_;
  }
  
}

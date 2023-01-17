//
//  is_land.hpp
//  ikan
//
//  Created by Ashish . on 16/01/23.
//

#pragma once

#include "body.hpp"
#include "physics_math.hpp"
#include "time_setup.hpp"

namespace physics {
  
  class Contact;
  class Joint;
  class StackAllocator;
  class ContactListener;
  struct ContactVelocityConstraint;
  struct Profile;

  /// This is an internal class.
  class Island {
  public:
    Island(int32_t bodyCapacity, int32_t contactCapacity, int32_t jointCapacity,
           StackAllocator* allocator, ContactListener* listener);
    ~Island();
    
    void Clear() {
      body_count_ = 0;
      contact_count_ = 0;
      join_count_ = 0;
    }
    
    void Solve(Profile* profile, const TimeStep& step, const Vec2& gravity, bool allowSleep);
    
    void SolveTOI(const TimeStep& subStep, int32_t toiIndexA, int32_t toiIndexB);
    
    void Add(Body* body) {
      IK_ASSERT(body_count_ < body_capacity_);
      body->is_land_index_ = body_count_;
      bodies_[body_count_] = body;
      ++body_count_;
    }
    
    void Add(Contact* contact) {
      IK_ASSERT(contact_count_ < contact_capacity_);
      contacts_[contact_count_++] = contact;
    }
    
    void Add(Joint* joint) {
      IK_ASSERT(join_count_ < joint_capacity_);
      joints_[join_count_++] = joint;
    }
    
    void Report(const ContactVelocityConstraint* constraints);
    
    // -----------------------
    // Member Variables
    // -----------------------
    StackAllocator* allocator_;
    ContactListener* listner_;
    
    Body** bodies_;
    Contact** contacts_;
    Joint** joints_;
    
    Position* positions_;
    Velocity* velocities_;
    
    int32_t body_count_;
    int32_t join_count_;
    int32_t contact_count_;
    
    int32_t body_capacity_;
    int32_t contact_capacity_;
    int32_t joint_capacity_;
  };

  
}

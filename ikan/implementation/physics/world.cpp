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
  m_gravity(gravity),
  m_destructionListener(nullptr), m_debugDraw(nullptr), m_bodyList(nullptr), m_jointList(nullptr),
  m_bodyCount(0), m_jointCount(0),
  m_warmStarting(true), m_continuousPhysics(true), m_subStepping(false),
  m_stepComplete(true), m_allowSleep(true),
    
  m_newContacts(false), m_locked(false), m_clearForces(true),
  m_inv_dt0(0.0f) {
    contact_manager.allocator_ = &m_blockAllocator;
    memset(&m_profile, 0, sizeof(Profile));
  }
  
  World::~World() {
    // Some shapes allocate using b2Alloc.
    Body* b = m_bodyList;
    while (b) {
      Body* bNext = b->next_;
      
      Fixture* f = b->fixture_list_;
      while (f) {
        Fixture* fNext = f->next_;
        f->proxy_count_ = 0;
        f->Destroy(&m_blockAllocator);
        f = fNext;
      }
      
      b = bNext;
    }
  }
  
  Body* World::CreateBody(const BodyDef* def) {
    IK_ASSERT(IsLocked() == false);
    
    void* mem = m_blockAllocator.Allocate(sizeof(Body));
    Body* b = new (mem) Body(def, this);
    
    // Add to world doubly linked list.
    b->prev_ = nullptr;
    b->next_ = m_bodyList;
    if (m_bodyList) {
      m_bodyList->prev_ = b;
    }
    m_bodyList = b;
    ++m_bodyCount;
    
    return b;
  }
  
  void World::Step(float dt, int32_t velocity_iterations, int32_t position_iterations) {
    Timer step_timer;
    
    // If new fixtures were added, we need to find the new contacts.
    if (m_newContacts) {
      contact_manager.FindNewContacts();
      m_newContacts = false;
    }

    m_locked = true;

    TimeStep step;
    step.dt = dt;
    step.velocity_iterations  = velocity_iterations;
    step.position_iterations = position_iterations;
    if (dt > 0.0f) {
      step.inv_dt = 1.0f / dt;
    } else {
      step.inv_dt = 0.0f;
    }

    step.dt_ratio = m_inv_dt0 * dt;    
    step.warm_starting = m_warmStarting;

    // Update contacts. This is where some contacts are destroyed.
    {
      Timer timer;
      contact_manager.Collide();
      m_profile.collide = timer.GetMilliseconds();
    }
//    
//    // Integrate velocities, solve velocity constraints, and integrate positions.
//    if (m_stepComplete && step.dt > 0.0f) {
//      Timer timer;
//      Solve(step);
//      m_profile.solve = timer.GetMilliseconds();
//    }


  }

  
}

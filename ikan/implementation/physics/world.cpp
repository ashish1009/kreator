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
    if (IsLocked()) {
      return nullptr;
    }
    
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
  
  void World::Step(float dt, int32_t velocity_iterations, int32_t position_tterations) {
    Timer step_timer;
  }

  
}

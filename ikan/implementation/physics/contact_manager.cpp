//
//  contact_manager.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "contact_manager.hpp"
#include "contact.hpp"
#include "fixture.hpp"
#include "world_callbacks.hpp"

namespace physics {

  ContactFilter _defaultFilter;
  ContactListener _defaultListener;
  
  ContactManager::ContactManager() {
    contact_list_ = nullptr;
    contact_count_ = 0;
    contact_filter_ = &_defaultFilter;
    contact_listener_ = &_defaultListener;
    allocator_ = nullptr;
  }
  
  void ContactManager::Destroy(Contact* c) {
    Fixture* fixtureA = c->GetFixtureA();
    Fixture* fixtureB = c->GetFixtureB();
    Body* bodyA = fixtureA->GetBody();
    Body* bodyB = fixtureB->GetBody();
    
    if (contact_listener_ && c->IsTouching()) {
      contact_listener_->EndContact(c);
    }
    
    // Remove from the world.
    if (c->prev_) {
      c->prev_->next_ = c->next_;
    }
    
    if (c->next_) {
      c->next_->prev_ = c->prev_;
    }
    
    if (c == contact_list_) {
      contact_list_ = c->next_;
    }
    
    // Remove from body 1
    if (c->node_a_.prev) {
      c->node_a_.prev->next = c->node_a_.next;
    }
    
    if (c->node_a_.next) {
      c->node_a_.next->prev = c->node_a_.prev;
    }
    
    if (&c->node_a_ == bodyA->contact_list_) {
      bodyA->contact_list_ = c->node_a_.next;
    }
    
    // Remove from body 2
    if (c->node_b_.prev) {
      c->node_b_.prev->next = c->node_b_.next;
    }
    
    if (c->node_b_.next) {
      c->node_b_.next->prev = c->node_b_.prev;
    }
    
    if (&c->node_b_ == bodyB->contact_list_) {
      bodyB->contact_list_ = c->node_b_.next;
    }
    
    // Call the factory.
    Contact::Destroy(c, allocator_);
    --contact_count_;
  }
  
  // This is the top level collision call for the time step. Here
  // all the narrow phase collision is processed for the world
  // contact list.
  void ContactManager::Collide() {
    // Update awake contacts.
    Contact* c = contact_list_;
    while (c) {
      Fixture* fixtureA = c->GetFixtureA();
      Fixture* fixtureB = c->GetFixtureB();
      int32_t indexA = c->GetChildIndexA();
      int32_t indexB = c->GetChildIndexB();
      Body* bodyA = fixtureA->GetBody();
      Body* bodyB = fixtureB->GetBody();
      
      // Is this contact flagged for filtering?
      if (c->flags_ & Contact::FilterFlag) {
        // Should these bodies collide?
        if (bodyB->ShouldCollide(bodyA) == false) {
          Contact* cNuke = c;
          c = cNuke->GetNext();
          Destroy(cNuke);
          continue;
        }
        
        // Check user filtering.
        if (contact_filter_ && contact_filter_->ShouldCollide(fixtureA, fixtureB) == false) {
          Contact* cNuke = c;
          c = cNuke->GetNext();
          Destroy(cNuke);
          continue;
        }
        
        // Clear the filtering flag.
        c->flags_ &= ~Contact::FilterFlag;
      }
      
      bool activeA = bodyA->IsAwake() && bodyA->type_ != BodyType::StaticBody;
      bool activeB = bodyB->IsAwake() && bodyB->type_ != BodyType::StaticBody;
      
      // At least one body must be awake and it must be dynamic or kinematic.
      if (activeA == false && activeB == false) {
        c = c->GetNext();
        continue;
      }
      
      int32_t proxyIdA = fixtureA->proxies_[indexA].proxy_id;
      int32_t proxyIdB = fixtureB->proxies_[indexB].proxy_id;
      bool overlap = broad_phase_.TestOverlap(proxyIdA, proxyIdB);
      
      // Here we destroy contacts that cease to overlap in the broad-phase.
      if (overlap == false) {
        Contact* cNuke = c;
        c = cNuke->GetNext();
        Destroy(cNuke);
        continue;
      }
      
      // The contact persists.
      c->Update(contact_listener_);
      c = c->GetNext();
    }
  }

  void ContactManager::FindNewContacts() {
    broad_phase_.UpdatePairs(this);
  }

  void ContactManager::AddPair(void* proxyUserDataA, void* proxyUserDataB) {
    FixtureProxy* proxyA = (FixtureProxy*)proxyUserDataA;
    FixtureProxy* proxyB = (FixtureProxy*)proxyUserDataB;

    Fixture* fixtureA = proxyA->fixture;
    Fixture* fixtureB = proxyB->fixture;

    int32_t indexA = proxyA->child_index;
    int32_t indexB = proxyB->child_index;

    Body* bodyA = fixtureA->GetBody();
    Body* bodyB = fixtureB->GetBody();

    // Are the fixtures on the same body?
    if (bodyA == bodyB) {
      return;
    }

    // TODO_ERIN use a hash table to remove a potential bottleneck when both
    // bodies have a lot of contacts.
    // Does a contact already exist?
    ContactEdge* edge = bodyB->GetContactList();
    while (edge) {
      if (edge->other == bodyA) {
        Fixture* fA = edge->contact->GetFixtureA();
        Fixture* fB = edge->contact->GetFixtureB();
        int32_t iA = edge->contact->GetChildIndexA();
        int32_t iB = edge->contact->GetChildIndexB();

        if (fA == fixtureA && fB == fixtureB && iA == indexA && iB == indexB) {
          // A contact already exists.
          return;
        }

        if (fA == fixtureB && fB == fixtureA && iA == indexB && iB == indexA) {
          // A contact already exists.
          return;
        }
      }

      edge = edge->next;
    }

    // Does a joint override collision? Is at least one body dynamic?
    if (bodyB->ShouldCollide(bodyA) == false) {
      return;
    }

    // Check user filtering.
    if (contact_filter_ && contact_filter_->ShouldCollide(fixtureA, fixtureB) == false) {
      return;
    }

    // Call the factory.
    Contact* c = Contact::Create(fixtureA, indexA, fixtureB, indexB, allocator_);
    if (c == nullptr) {
      return;
    }

    // Contact creation may swap fixtures.
    fixtureA = c->GetFixtureA();
    fixtureB = c->GetFixtureB();
    indexA = c->GetChildIndexA();
    indexB = c->GetChildIndexB();
    bodyA = fixtureA->GetBody();
    bodyB = fixtureB->GetBody();

    // Insert into the world.
    c->prev_ = nullptr;
    c->next_ = contact_list_;
    if (contact_list_ != nullptr) {
      contact_list_->prev_ = c;
    }
    contact_list_ = c;

    // Connect to island graph.

    // Connect to body A
    c->node_a_.contact = c;
    c->node_a_.other = bodyB;

    c->node_a_.prev = nullptr;
    c->node_a_.next = bodyA->contact_list_;
    if (bodyA->contact_list_ != nullptr) {
      bodyA->contact_list_->prev = &c->node_a_;
    }
    bodyA->contact_list_ = &c->node_a_;

    // Connect to body B
    c->node_b_.contact = c;
    c->node_b_.other = bodyA;

    c->node_b_.prev = nullptr;
    c->node_b_.next = bodyB->contact_list_;
    if (bodyB->contact_list_ != nullptr) {
      bodyB->contact_list_->prev = &c->node_b_;
    }
    bodyB->contact_list_ = &c->node_b_;

    ++contact_count_;
  }

}

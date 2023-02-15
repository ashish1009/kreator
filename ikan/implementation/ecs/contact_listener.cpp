//
//  contact_listner.cpp
//  ikan
//
//  Created by Ashish . on 15/02/23.
//

#include "contact_listener.hpp"
#include "entity.hpp"
#include "core_components.hpp"

#include <box2d/box2d.h>

namespace ikan {
  
  void ContactListner::BeginContact(b2Contact* contact) {
    Entity* entity_a = (Entity*)contact->GetFixtureA()->GetUserData().pointer;
    Entity* entity_b = (Entity*)contact->GetFixtureB()->GetUserData().pointer;
    
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    
    glm::vec2 a_normal = {world_manifold.normal.x, world_manifold.normal.y};
    glm::vec2 b_normal = a_normal * -1.0f;
    
    if (entity_a and entity_a->GetScene() and entity_a->HasComponent<NativeScriptComponent>())
      entity_a->GetComponent<NativeScriptComponent>().script->BeginCollision(entity_b, contact, a_normal);
    if (entity_b and entity_b->GetScene() and entity_b->HasComponent<NativeScriptComponent>())
      entity_b->GetComponent<NativeScriptComponent>().script->BeginCollision(entity_a, contact, b_normal);
  }
  
  void ContactListner::EndContact(b2Contact* contact) {
    Entity* entity_a = (Entity*)contact->GetFixtureA()->GetUserData().pointer;
    Entity* entity_b = (Entity*)contact->GetFixtureB()->GetUserData().pointer;
    
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    
    glm::vec2 a_normal = {world_manifold.normal.x, world_manifold.normal.y};
    glm::vec2 b_normal = a_normal * -1.0f;
    
    if (entity_a and entity_a->GetScene() and entity_a->HasComponent<NativeScriptComponent>())
      entity_a->GetComponent<NativeScriptComponent>().script->EndCollision(entity_b, contact, a_normal);
    if (entity_b and entity_b->GetScene() and entity_b->HasComponent<NativeScriptComponent>())
      entity_b->GetComponent<NativeScriptComponent>().script->EndCollision(entity_a, contact, b_normal);
  }
  
  void ContactListner::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    Entity* entity_a = (Entity*)contact->GetFixtureA()->GetUserData().pointer;
    Entity* entity_b = (Entity*)contact->GetFixtureB()->GetUserData().pointer;
    
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    
    glm::vec2 a_normal = {world_manifold.normal.x, world_manifold.normal.y};
    glm::vec2 b_normal = a_normal * -1.0f;
    
    if (entity_a and entity_a->GetScene() and entity_a->HasComponent<NativeScriptComponent>())
      entity_a->GetComponent<NativeScriptComponent>().script->PreSolve(entity_b, contact, a_normal);
    if (entity_b and entity_b->GetScene() and entity_b->HasComponent<NativeScriptComponent>())
      entity_b->GetComponent<NativeScriptComponent>().script->PreSolve(entity_a, contact, b_normal);
  }
  
  void ContactListner::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    Entity* entity_a = (Entity*)contact->GetFixtureA()->GetUserData().pointer;
    Entity* entity_b = (Entity*)contact->GetFixtureB()->GetUserData().pointer;
    
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    
    glm::vec2 a_normal = {world_manifold.normal.x, world_manifold.normal.y};
    glm::vec2 b_normal = a_normal * -1.0f;
    
    if (entity_a and entity_a->GetScene() and entity_a->HasComponent<NativeScriptComponent>())
      entity_a->GetComponent<NativeScriptComponent>().script->PostSolve(entity_b, contact, a_normal);
    if (entity_b and entity_b->GetScene() and entity_b->HasComponent<NativeScriptComponent>())
      entity_b->GetComponent<NativeScriptComponent>().script->PostSolve(entity_a, contact, b_normal);
  }
  
}

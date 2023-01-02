//
//  entity.hpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#pragma once

// This file incldues the Entity class to store entt data

#include "scene/entt_scene.hpp"

namespace ikan {
  
  /// enTT Wrapper class to get, add .. APIs
  class Entity {
  public:
    Entity();
    /// This Constructor creates an Entity with handle and scene pointer
    /// - parameters:
    ///   - handle: handle of Entity. Scene (registry) takes care for this
    ///   - scene: Scene pointer to which this entity to be bound
    Entity(entt::entity handle, EnttScene* scene);
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(Entity);
    
    /// This is default entity Destrcutor
    ~Entity();
    
    // ----------------
    // Operators
    // ----------------
    /// This function returns Entity as entity handle
    operator entt::entity() const;

  private:
    entt::entity entity_handle_{ entt::null };
    EnttScene* scene_;
  };
  
}

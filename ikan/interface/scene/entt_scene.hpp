//
//  scene.hpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#pragma once

// This file includes the scene class to store data of active scene

#include <entt.hpp>

namespace ikan {

  // Forward declaration
  class Entity;

  class EnttScene {
  public:
    /// This Constructor creates the instance of Scene.
    EnttScene();
    /// This destructor destoyes the scene instance
    ~EnttScene();
    
    /// This function create and Entity and store in scene registry
    /// - Parameters:
    ///   - uuid: Unique ID of entity
    Entity CreateEntity(UUID uuid = UUID());
    /// This function destory the entity from scene registry
    /// - Parameter entity: entity to be destroyed
    void DestroyEntity(Entity entity);
    
    DELETE_COPY_MOVE_CONSTRUCTORS(EnttScene);
    
  private:
    // ------------------
    // Member variabls
    // ------------------
    // Registry to store the entity handles
    entt::registry registry_;
    
    friend class Entity;
  };
  
}

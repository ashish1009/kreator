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
    Entity CreateEntity();
    /// This function destory the entity from scene registry
    /// - Parameter entity: <#entity description#>
    void DestroyEntity(Entity entity);
    
  private:
    // ------------------
    // Member variabls
    // ------------------
    // Registry to store the entity handles
    entt::registry registry_;
  };
  
}

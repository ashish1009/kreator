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
    ///   - name: name of entity
    ///   - uuid: Unique ID of entity
    Entity CreateEntity(const std::string& name = "Unknown Entity",
                        UUID uuid = UUID());
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
    std::unordered_map<entt::entity, Entity> entity_id_map_;

    // Number of Entity stored in Scene and Max ID given to Entity
    uint32_t num_entities_ = 0, max_entity_id_ = -1;

    friend class Entity;
  };
  
}

//
//  dummy_scene.hpp
//  ikan
//
//  Created by Ashish . on 09/02/23.
//

#pragma once

#include <entt.hpp>

namespace ikan {
  
  // Forward declaration
  class Entity;
  class Scence {
  public:
    /// This function create and Entity and store in scene registry
    /// - Parameters:
    ///   - name: name of entity
    ///   - uuid: Unique ID of entity
    [[nodiscard]] Entity CreateEntity(const std::string& name = "Unknown Entity", UUID uuid = UUID());
    /// This function destory the entity from scene registry
    /// - Parameter entity: entity to be destroyed
    void DestroyEntity(Entity entity);
    /// This function duplicate the entity and copy all the components
    /// - Parameter entity: entity to be destroyed
    void DuplicateEntity(Entity entity);

    /// This function returns the entity Ref from its id
    /// - Parameter id: entity ID
    Entity* GetEnitityFromId(int32_t id);
    /// This function returns the reference of registry
    entt::registry& GetRegistry();
    /// This function returns the number of Entities stored in Scene
    uint32_t GetNumEntities() const;
    /// This function returns the Max Entity ID given to scene
    uint32_t GetMaxEntityId() const;

  private:
    /// - Parameters:
    ///   - name: name of entity
    ///   - uuid: Unique ID of entity
    Entity CreateNewEmptyEntity(const std::string& name, UUID uuid);

  protected:
    // Registry to store the entity handles
    entt::registry registry_;
    std::unordered_map<entt::entity, Entity> entity_id_map_;

    // Number of Entity stored in Scene and Max ID given to Entity
    uint32_t num_entities_ = 0, max_entity_id_ = -1;
    
    friend class Entity;
    friend class ScenePanelManager;
    friend class SceneSerializer;
  };
  
}

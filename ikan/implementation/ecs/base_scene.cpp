//
//  dummy_scene.cpp
//  ikan
//
//  Created by Ashish . on 09/02/23.
//

#include "base_scene.hpp"
#include "entity.hpp"
#include "core_components.hpp"

namespace ikan {
  
  template<typename Component>
  /// Copy the entity components
  /// - Parameters:
  ///   - dst: Destination entity
  ///   - src: Source entity
  static void CopyComponentIfExist(Entity& dst, Entity& src) {
    if (src.HasComponent<Component>())
      dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
  }

  Entity Scene::CreateEntity(const std::string& name, UUID uuid) {
    Entity entity = CreateNewEmptyEntity(name, uuid);
    
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    
    // Debug Logs
    IK_CORE_TRACE(LogModule::EnttScene, "Stored Entity in Scene");
    IK_CORE_TRACE(LogModule::EnttScene, "  Name | {0}", entity.GetComponent<TagComponent>().tag.c_str());
    IK_CORE_TRACE(LogModule::EnttScene, "  ID   | {0}", entity.GetComponent<IDComponent>().id);
    IK_CORE_TRACE(LogModule::EnttScene, "  Number of entities Added in Scene | {0}", ++num_entities_);
    IK_CORE_TRACE(LogModule::EnttScene, "  Max ID given to entity            | {0}", max_entity_id_);
    
    return entity;
  }
  
  void Scene::DestroyEntity(Entity entity) {
    IK_CORE_WARN(LogModule::EnttScene, "Removed Entity from Scene");
    IK_CORE_WARN(LogModule::EnttScene, "  Name | {0}", entity.GetComponent<TagComponent>().tag.c_str());
    IK_CORE_WARN(LogModule::EnttScene, "  ID   | {0}", entity.GetComponent<IDComponent>().id);
    IK_CORE_WARN(LogModule::EnttScene, "  Number of entities Added in Scene | {0}", --num_entities_);
    
    // Delete the eneity from the map
    entity_id_map_.erase(entity);
    
    registry_.destroy(entity);
  }
  
  void Scene::DuplicateEntity(Entity entity) {
    Entity new_entity = CreateNewEmptyEntity("", UUID());
    
    // Copy Components
    CopyComponentIfExist<TagComponent>(new_entity, entity);
    CopyComponentIfExist<TransformComponent>(new_entity, entity);
    CopyComponentIfExist<CameraComponent>(new_entity, entity);
    CopyComponentIfExist<QuadComponent>(new_entity, entity);
    CopyComponentIfExist<CircleComponent>(new_entity, entity);
    
    // Disable highlight from selected Entity
    if (*selected_entity_ == entity) {
      if (new_entity.HasComponent<QuadComponent>())
        new_entity.GetComponent<QuadComponent>().color.a += highlight_entity_alpha_diff_;
    }
  }
  
  Entity Scene::CreateNewEmptyEntity(const std::string &name, UUID uuid) {
    Entity entity {registry_.create(), this};
    
    // Assert if this entity id is already present in scene entity map
    IK_ASSERT((entity_id_map_.find(entity) == entity_id_map_.end()), "Entity Already Added");
    
    // Add the Mendatory components
    entity.AddComponent<IDComponent>(uuid);
    
    // Store the entity in the entity uuid map. We Copy the Entity
    entity_id_map_[entity] = std::move(entity);
    
    // Updating the Max entity ID
    max_entity_id_ = entity;
    
    return entity;
  }
  
  Entity* Scene::GetEnitityFromId(int32_t id) {
    if (entity_id_map_.find((entt::entity)id) != entity_id_map_.end())
      return &entity_id_map_.at((entt::entity)id);
    return nullptr;
  }
  
  void Scene::SetSelectedEntity(Entity* entity) { selected_entity_ = entity; }
  Entity* Scene::GetSelectedEntity() { return selected_entity_; }

  entt::registry& Scene::GetRegistry() { return registry_; }
  uint32_t Scene::GetNumEntities() const { return num_entities_; }
  uint32_t Scene::GetMaxEntityId() const { return max_entity_id_; }

}

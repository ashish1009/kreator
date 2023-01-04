//
//  scene.cpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#include "entt_scene.hpp"
#include "scene/entity.hpp"
#include "scene/core_components.hpp"
#include "renderer/utils/batch_2d_renderer.hpp"

namespace ikan {
  
  EnttScene::EnttScene() {
    IK_CORE_INFO(LogModule::EnttScene, "Creating Scene ...");
    
    Entity e1 = CreateEntity();
  }
  
  EnttScene::~EnttScene() {
    IK_CORE_WARN(LogModule::EnttScene, "Destroying Scene!!!");
  }
  
  Entity EnttScene::CreateEntity(const std::string& name, UUID uuid) {
    Entity entity {registry_.create(), this};
    
    // Assert if this entity id is already present in scene entity map
    IK_ASSERT((entity_id_map_.find(entity) == entity_id_map_.end()),
              "Entity Already Added");
    
    // Store the entity in the entity uuid map. We Copy the Entity
    entity_id_map_[entity] = std::move(entity);

    // Add the Mendatory components
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    
    // Updating the Max entity ID
    max_entity_id_ = entity;

    // Debug Logs
    IK_CORE_TRACE(LogModule::EnttScene, "Stored Entity in Scene");
    IK_CORE_TRACE(LogModule::EnttScene, "  Name | {0}", entity.GetComponent<TagComponent>().tag.c_str());
    IK_CORE_TRACE(LogModule::EnttScene, "  ID   | {0}", entity.GetComponent<IDComponent>().id);
    IK_CORE_TRACE(LogModule::EnttScene, "  Number of entities Added in Scene | {0}", ++num_entities_);
    IK_CORE_TRACE(LogModule::EnttScene, "  Max ID given to entity            | {0}", max_entity_id_);

    return entity;
  }
  
  void EnttScene::DestroyEntity(Entity entity) {
    IK_CORE_WARN(LogModule::EnttScene, "Removed Entity from Scene");
    IK_CORE_WARN(LogModule::EnttScene, "  Name | {0}", entity.GetComponent<TagComponent>().tag.c_str());
    IK_CORE_WARN(LogModule::EnttScene, "  ID   | {0}", entity.GetComponent<IDComponent>().id);
    IK_CORE_WARN(LogModule::EnttScene, "  Number of entities Added in Scene | {0}", --num_entities_);

    // Delete the eneity from the map
    entity_id_map_.erase(entity);

    registry_.destroy(entity);
  }
  
  void EnttScene::Update(Timestep ts) {
    editor_camera_.Update(ts);
    
    BatchRenderer::BeginBatch(editor_camera_.GetViewProjection(), editor_camera_.GetView());
    BatchRenderer::DrawQuad(Math::GetTransformMatrix({0, 0, 0},
                                                     {0, 0, 0},
                                                     {1, 1, 1}),
                            {0.3, 0.4, 0.5, 1.0});
    BatchRenderer::EndBatch();
  }
  
  void EnttScene::EventHandler(Event& e) {
    editor_camera_.EventHandler(e);
  }
  
  void EnttScene::SetViewport(uint32_t width, uint32_t height) {
    editor_camera_.SetViewportSize(width, height);
  }

  void EnttScene::RenderGui() {
    
  }

}

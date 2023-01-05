//
//  scene.cpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#include "entt_scene.hpp"
#include "scene/entity.hpp"
#include "scene/core_components.hpp"
#include "editor/property_grid.hpp"
#include "renderer/utils/batch_2d_renderer.hpp"
#include "renderer/utils/renderer.hpp"
#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  EnttScene::EnttScene() {
    IK_CORE_INFO(LogModule::EnttScene, "Creating Scene ...");
    
    // Set the Scene state and register their corresponding Functions
    if (state_ == State::Edit)
      EditScene();
    else if (state_ == State::Play)
      PlayScene();
    else
      IK_ASSERT(false, "Invalid State");

    // ------------
    // TODO: Temp
    // ------------
    Entity e1 = CreateEntity();
    e1.AddComponent<QuadComponent>();

    Entity e2 = CreateEntity();
    e2.GetComponent<TransformComponent>().translation = { 2, 0, 0};
    e2.AddComponent<CircleComponent>();
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
    update_(ts);
  }
  
  void EnttScene::UpdateEditor(Timestep ts) {
    editor_camera_.Update(ts);
    
    // Render 2D
    BatchRenderer::BeginBatch(editor_camera_.GetViewProjection(), editor_camera_.GetView());
    
    auto quad_view = registry_.view<TransformComponent, QuadComponent>();
    // For all Mesg entity
    for (const auto& quad_entity : quad_view) {
      const auto& [transform_component, quad_component] = quad_view.get<TransformComponent, QuadComponent>(quad_entity);
      BatchRenderer::DrawQuad(transform_component.GetTransform(),
                              quad_component.color,
                              (uint32_t)quad_entity);
    } // for (const auto& entity : mesh_view)
    
    auto circle_view = registry_.view<TransformComponent, CircleComponent>();
    // For all Mesg entity
    for (const auto& ciecle_entity : circle_view) {
      const auto& [transform_component, circle_component] = circle_view.get<TransformComponent, CircleComponent>(ciecle_entity);
      BatchRenderer::DrawCircle(transform_component.GetTransform(),
                                circle_component.color,
                                circle_component.thickness,
                                circle_component.fade);
    } // for (const auto& entity : mesh_view)

    BatchRenderer::EndBatch();
  }
  
  void EnttScene::UpdateRuntime(Timestep ts) {
  }
  
  void EnttScene::EventHandler(Event& event) {
    event_handler_(event);
  }
  
  void EnttScene::EventHandlerEditor(Event& event) {
    editor_camera_.EventHandler(event);
  }
  
  void EnttScene::EventHandlerRuntime(Event& event) {
  }

  void EnttScene::RenderGui() {
    render_imgui_();
    
    // Texture for Play and Pause button
    static std::shared_ptr<Texture> pause_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/pause.png"));
    static std::shared_ptr<Texture> play_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/play.png"));
    
    // Play Pause Buttom
    ImGui::Begin("Play/Pause",
                 nullptr, // No Open Flag
                 ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse);
    
    // Update the texture id based on the state of scene
    uint32_t tex_id = state_ == State::Edit ? play_texture->GetRendererID() : pause_texture->GetRendererID();
    
    float size = ImGui::GetWindowHeight() - 12.0f; // 12 just random number
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    
    // Button action
    if (PropertyGrid::ImageButton("Play/Pause", tex_id, { size, size })) {
      if (state_ == State::Edit)
        PlayScene();
      else
        EditScene();
    }
    
    ImGui::End();

  }
  
  void EnttScene::RenderImguiEditor() {
    editor_camera_.RendererGui();
  }
  
  void EnttScene::RenderImguiRuntime() {
  }
  
  void EnttScene::SetViewport(uint32_t width, uint32_t height) {
    editor_camera_.SetViewportSize(width, height);
  }
  
  void EnttScene::PlayScene() {
    IK_CORE_TRACE(LogModule::EnttScene, "Scene is Set to Play");
    
    state_ = State::Play;
    update_ = std::bind(&EnttScene::UpdateRuntime, this, std::placeholders::_1);
    event_handler_ = std::bind(&EnttScene::EventHandlerRuntime, this, std::placeholders::_1);
    render_imgui_ = std::bind(&EnttScene::RenderImguiRuntime, this);
  }
  
  void EnttScene::EditScene() {
    IK_CORE_TRACE(LogModule::EnttScene, "Scene is Set to Edit");
    
    state_ = State::Edit;
    update_ = std::bind(&EnttScene::UpdateEditor, this, std::placeholders::_1);
    event_handler_ = std::bind(&EnttScene::EventHandlerEditor, this, std::placeholders::_1);
    render_imgui_ = std::bind(&EnttScene::RenderImguiEditor, this);    
  }

}

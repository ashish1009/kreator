//
//  scene.cpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#include "entt_scene.hpp"
#include "ecs/entity.hpp"
#include "ecs/core_components.hpp"
#include "ecs/scriptable_entity.hpp"
#include "editor/property_grid.hpp"
#include "renderer/utils/batch_2d_renderer.hpp"
#include "renderer/utils/renderer.hpp"
#include "renderer/utils/aabb_renderer.hpp"
#include "renderer/graphics/texture.hpp"

#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"

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
  
  EnttScene::EnttScene(const std::string& file_path)
  : file_path_(file_path), name_(StringUtils::GetNameFromFilePath(file_path)) {
    IK_CORE_INFO(LogModule::EnttScene, "Creating Scene ...");
    IK_CORE_INFO(LogModule::EnttScene, "  Name | {0}", name_);

    // Set the Scene state and register their corresponding Functions
    if (state_ == State::Edit)
      EditScene();
    else if (state_ == State::Play)
      PlayScene();
    else
      IK_ASSERT(false, "Invalid State");
  }
  
  EnttScene::~EnttScene() {
    IK_CORE_WARN(LogModule::EnttScene, "Destroying Scene!!!");
    IK_CORE_WARN(LogModule::EnttScene, "  Name | {0}", name_);
  }
  
  Entity EnttScene::CreateEntity(const std::string& name, UUID uuid) {
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
  
  void EnttScene::DestroyEntity(Entity entity) {
    IK_CORE_WARN(LogModule::EnttScene, "Removed Entity from Scene");
    IK_CORE_WARN(LogModule::EnttScene, "  Name | {0}", entity.GetComponent<TagComponent>().tag.c_str());
    IK_CORE_WARN(LogModule::EnttScene, "  ID   | {0}", entity.GetComponent<IDComponent>().id);
    IK_CORE_WARN(LogModule::EnttScene, "  Number of entities Added in Scene | {0}", --num_entities_);
    
    // Delete the eneity from the map
    entity_id_map_.erase(entity);
    
    registry_.destroy(entity);
  }
  
  Entity EnttScene::DuplicateEntity(Entity entity) {
    Entity new_entity = CreateNewEmptyEntity("", UUID());
    
    // Copy Components
    CopyComponentIfExist<TagComponent>(new_entity, entity);
    CopyComponentIfExist<TransformComponent>(new_entity, entity);
    CopyComponentIfExist<CameraComponent>(new_entity, entity);
    CopyComponentIfExist<QuadComponent>(new_entity, entity);
    CopyComponentIfExist<CircleComponent>(new_entity, entity);
    CopyComponentIfExist<RigidBodyComponent>(new_entity, entity);
    CopyComponentIfExist<BoxColloiderComponent>(new_entity, entity);
    CopyComponentIfExist<AnimationComponent>(new_entity, entity);

    return new_entity;
  }
  
  Entity EnttScene::CreateNewEmptyEntity(const std::string &name, UUID uuid) {
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
  
  Entity* EnttScene::GetEnitityFromId(int32_t id) {
    if (entity_id_map_.find((entt::entity)id) != entity_id_map_.end())
      return &entity_id_map_.at((entt::entity)id);
    return nullptr;
  }
  
  void EnttScene::Update(Timestep ts) {
    // Update the primary scene camera for run time rendering
    UpdatePrimaryCameraData();
    
    // Update scripts
    InstantiateScript(ts);

    update_(ts);
  }
  
  void EnttScene::UpdateEditor(Timestep ts) {
    if (use_editor_camera_) {
      editor_camera_.Update(ts);
      Render2DEntities(editor_camera_.GetViewProjection());
    } else {
      UpdateRuntime(ts);
    }
  }
  
  void EnttScene::UpdateRuntime(Timestep ts) {    
    if (state_ == State::Play) { // TODO: Temp check till we use scene camera in editor mode
      // Physics
      const int32_t velocity_iteration = 6;
      const int32_t position_iteration = 2;
      
      physics_world_->Step(ts, velocity_iteration, position_iteration);
      
      // Get Transform
      auto view = registry_.view<RigidBodyComponent>();
      for (auto e : view) {
        Entity& entity = entity_id_map_.at(e);//{ e, this };

        auto& transform = entity.GetComponent<TransformComponent>();
        auto& rb2d = entity.GetComponent<RigidBodyComponent>();

        b2Body* body = (b2Body*)rb2d.runtime_body;
        if (body != nullptr) {
          const auto& position = body->GetPosition();

          transform.UpdateRotation_Z(body->GetAngle());
          transform.UpdateTranslation({position.x, position.y, 0.0f});
        }
      }
    }
    
    if (primary_camera_data_.scene_camera) {
      Render2DEntities(primary_camera_data_.scene_camera->GetProjection() * glm::inverse(primary_camera_data_.transform_matrix));
    }
  }
  
  void EnttScene::RuntimeStart() {
    physics_world_ = new b2World({0.0f, -9.8f});
    auto view = registry_.view<RigidBodyComponent>();
    for (auto e : view) {
      Entity entity = { e, this };
      auto& transform = entity.GetComponent<TransformComponent>();
      auto& rb2d = entity.GetComponent<RigidBodyComponent>();
      
      b2BodyDef body_def;
      body_def.type = rb2d.type;
      body_def.position.Set(transform.Translation().x, transform.Translation().y);
      body_def.angle = transform.Rotation().z;
      
      b2Body* body = physics_world_->CreateBody(&body_def);
      body->SetFixedRotation(rb2d.fixed_rotation);
      
      rb2d.runtime_body = body;
      
      if (entity.HasComponent<BoxColloiderComponent>()) {
        auto& bc2d = entity.GetComponent<BoxColloiderComponent>();
        
        b2PolygonShape polygon_shape;
        polygon_shape.SetAsBox(bc2d.size.x * transform.Scale().x, bc2d.size.y * transform.Scale().y);
        
        b2FixtureDef fixture_def;
        fixture_def.shape = & polygon_shape;
        fixture_def.density = bc2d.density;
        fixture_def.friction = bc2d.friction;
        fixture_def.restitution = bc2d.restitution;
        fixture_def.restitutionThreshold = bc2d.restitution_threshold;
        
        body->CreateFixture(&fixture_def);
      }
      
      if (entity.HasComponent<CircleColloiderComponent>()) {
        auto& cc2d = entity.GetComponent<CircleColloiderComponent>();
        
        b2CircleShape circle_shape;
        
        circle_shape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
        circle_shape.m_radius = transform.Scale().x * cc2d.radius;
        
        b2FixtureDef fixture_def;
        fixture_def.shape = & circle_shape;
        fixture_def.density = cc2d.density;
        fixture_def.friction = cc2d.friction;
        fixture_def.restitution = cc2d.restitution;
        fixture_def.restitutionThreshold = cc2d.restitution_threshold;
        
        body->CreateFixture(&fixture_def);
      }
    }
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
  }
  
  void EnttScene::RenderImguiEditor() {
    if (use_editor_camera_)
      editor_camera_.RendererGui(&setting_.editor_camera);
    
    // Scene Debugger
    if (setting_.scene_controller) {
      ImGui::Begin("Scene Controller", &setting_.scene_controller);
      ImGui::PushID("Scene Controller");
      
      PropertyGrid::CheckBox("Use Editor Camera", use_editor_camera_, ImGui::GetContentRegionAvailWidth() / 2);
      
      ImGui::PopID();
      ImGui::End();
    }
  
    ImGui::Begin("Scene Data", &setting_.scene_data);
   
    // Render Scene Information
    ImGui::Text(" Scene | %s ", name_.c_str());
    std::string hovered_msg = "Num Entities   : " + std::to_string(num_entities_) + "\n" "Max Entity ID  : " + std::to_string(max_entity_id_);
    PropertyGrid::HoveredMsg(hovered_msg.c_str());
    
    ImGui::End();
  }
  
  void EnttScene::RenderImguiRuntime() {
    if (primary_camera_data_.scene_camera) {
      ImGui::Begin("Primary Camera");
      ImGui::PushID("Primary Camera");
      
      primary_camera_data_.scene_camera->RenderGui();
      ImGui::Separator();
      primary_camera_data_.transform_comp->RenderGui();
      
      ImGui::PopID();
      ImGui::End();
    }
  }
  
  void EnttScene::SetViewport(uint32_t width, uint32_t height) {
    // TODO: for both running and editor???
    if (viewport_width_ == width and viewport_height_ == height)
      return;
    
    // Update viewport
    viewport_width_ = width;
    viewport_height_ = height;

    // editor
    editor_camera_.SetViewportSize(width, height);
    
    // Update the viewport of scene cameras
    auto view = registry_.view<CameraComponent>();
    for (auto entity : view) {
      auto& cc = view.get<CameraComponent>(entity);
      if (!cc.is_fixed_aspect_ratio)
        cc.camera->SetViewportSize(width, height);
    }
  }
  
  void EnttScene::PlayScene() {
    IK_CORE_TRACE(LogModule::EnttScene, "Scene is Set to Play");
    
    state_ = State::Play;
    update_ = std::bind(&EnttScene::UpdateRuntime, this, std::placeholders::_1);
    event_handler_ = std::bind(&EnttScene::EventHandlerRuntime, this, std::placeholders::_1);
    render_imgui_ = std::bind(&EnttScene::RenderImguiRuntime, this);
    
    RuntimeStart();
  }
  
  void EnttScene::EditScene() {
    IK_CORE_TRACE(LogModule::EnttScene, "Scene is Set to Edit");
    
    state_ = State::Edit;
    update_ = std::bind(&EnttScene::UpdateEditor, this, std::placeholders::_1);
    event_handler_ = std::bind(&EnttScene::EventHandlerEditor, this, std::placeholders::_1);
    render_imgui_ = std::bind(&EnttScene::RenderImguiEditor, this);
    
    delete physics_world_;
    physics_world_ = nullptr;
  }
  
  void EnttScene::UpdatePrimaryCameraData() {
    auto camera_view = registry_.view<TransformComponent, CameraComponent>();
    for (auto& camera_entity : camera_view) {
      const auto& [transform_component, camera_component] = camera_view.get<TransformComponent, CameraComponent>(camera_entity);
      if (camera_component.is_primary) {
        primary_camera_data_.scene_camera = camera_component.camera.get();
        primary_camera_data_.position = transform_component.Translation();
        primary_camera_data_.transform_matrix = transform_component.GetTransform();
        
        primary_camera_data_.transform_comp = &transform_component;
        return;
      }
    }
    primary_camera_data_.scene_camera = nullptr;
  }
  
  void EnttScene::InstantiateScript(Timestep ts) {
    registry_.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
                                                 {
      for (auto& [name, script] : nsc.scrip_name_map) {
        if (!script->scene_) {
          script->entity_ = Entity{ entity, this };
          script->scene_ = this;
          
          script->Create();
        }
        
        if (state_ == State::Play)
          script->Update(ts);
      }
    });
  }
  
  void EnttScene::Render2DEntities(const glm::mat4& camera_view_projection_mat) {
    // Render 2D
    BatchRenderer::BeginBatch(camera_view_projection_mat);
    
    auto circle_view = registry_.view<TransformComponent, CircleComponent>();
    // For all circle entity
    for (const auto& circle_entity : circle_view) {
      const auto& [transform_component, circle_component] = circle_view.get<TransformComponent, CircleComponent>(circle_entity);
      if (circle_component.texture_comp.use and circle_component.texture_comp.component) {
        BatchRenderer::DrawCircle(transform_component.GetTransform(),
                                  circle_component.texture_comp.component,
                                  circle_component.color,
                                  circle_component.texture_comp.tiling_factor,
                                  circle_component.thickness,
                                  circle_component.fade,
                                  (uint32_t)circle_entity);
        
      } else {
        BatchRenderer::DrawCircle(transform_component.GetTransform(),
                                  circle_component.color,
                                  circle_component.thickness,
                                  circle_component.fade,
                                  (uint32_t)circle_entity);
      }
    } // for (const auto& entity : mesh_view)

    auto quad_view = registry_.view<TransformComponent, QuadComponent>();
    // For all quad entity
    for (const auto& quad_entity : quad_view) {
      const auto& [transform_component, quad_component] = quad_view.get<TransformComponent, QuadComponent>(quad_entity);
      if (quad_component.texture_comp.use and quad_component.texture_comp.component) {
        if (quad_component.texture_comp.use_sprite) {
          // Animation
          if (registry_.has<AnimationComponent>(quad_entity)) {
            auto& ac = registry_.get<AnimationComponent>(quad_entity);
            if (ac.animation and ac.is_sprite and ac.sprites.size() > 0) { // Sprite Animation
              if (ac.anim_idx >= ac.speed * ac.sprites.size() or ac.anim_idx < 1)
                ac.anim_idx = 0;
              BatchRenderer::DrawQuad(transform_component.GetTransform(),
                                      ac.sprites[ac.anim_idx / ac.speed],
                                      quad_component.color,
                                      (uint32_t)quad_entity);
              ac.anim_idx++;
            }
            else { // Sprite No Animation
              BatchRenderer::DrawQuad(transform_component.GetTransform(),
                                      quad_component.texture_comp.sprite,
                                      quad_component.color,
                                      (uint32_t)quad_entity);
            }
          }
          else { // Sprite No Animation
            BatchRenderer::DrawQuad(transform_component.GetTransform(),
                                    quad_component.texture_comp.sprite,
                                    quad_component.color,
                                    (uint32_t)quad_entity);
          }
        }
        else { // Texture Quad
          BatchRenderer::DrawQuad(transform_component.GetTransform(),
                                  quad_component.texture_comp.component,
                                  quad_component.color,
                                  quad_component.texture_comp.tiling_factor,
                                  (uint32_t)quad_entity);
        }
      } else { // Color Quad
        BatchRenderer::DrawQuad(transform_component.GetTransform(),
                                quad_component.color,
                                (uint32_t)quad_entity);
      }
    } // for (const auto& entity : mesh_view)
    
    auto sprite_view = registry_.view<TransformComponent, SpriteComponent>();
    // For all sprite entity
    for (const auto& sprite_entity : sprite_view) {
      const auto& [transform_component, sprite_component] = sprite_view.get<TransformComponent, SpriteComponent>(sprite_entity);
      BatchRenderer::DrawQuad(transform_component.GetTransform(),
                              sprite_component.sub_texture,
                              glm::vec4(1.0f),
                              (uint32_t)sprite_entity);
    } // for (const auto& entity : mesh_view)
    
    BatchRenderer::EndBatch();
  }
    
  void EnttScene::SetFilePath(const std::string& file_path) {
    file_path_ = file_path;
    name_ = StringUtils::GetNameFromFilePath(file_path_);
  }

  void EnttScene::SetSelectedEntity(Entity* entity) { selected_entity_ = entity; }
  Entity* EnttScene::GetSelectedEntity() { return selected_entity_; }
  
  entt::registry& EnttScene::GetRegistry() { return registry_; }
  uint32_t EnttScene::GetNumEntities() const { return num_entities_; }
  uint32_t EnttScene::GetMaxEntityId() const { return max_entity_id_; }

  EnttScene::Setting& EnttScene::GetSetting() { return setting_; }
  EditorCamera* EnttScene::GetEditorCamera() { return &editor_camera_; }
  bool EnttScene::IsEditing() const { return state_ == EnttScene::State::Edit; }
  const std::string& EnttScene::GetName() const { return name_; }
  const std::string& EnttScene::GetFilePath() const { return file_path_; }
  const CameraData& EnttScene::GetPrimaryCameraData() const { return primary_camera_data_; }
  bool EnttScene::UseEditorCamera() const { return use_editor_camera_; }
}

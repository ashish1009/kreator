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

#define DEBUG_DRAW 1

namespace ikan {
  
  template<typename... Component>
  static void CopyComponent(entt::registry& dst,
                            entt::registry& src,
                            const std::unordered_map<UUID, entt::entity>& enttMap) {
    ([&]()
     {
      auto view = src.view<Component>();
      for (auto src_entity : view) {
        entt::entity dst_entity = enttMap.at(src.get<IDComponent>(src_entity).id);
        
        auto& srcComponent = src.get<Component>(src_entity);
        dst.emplace_or_replace<Component>(dst_entity, srcComponent);
      }
    }(), ...);
  }
  
  template<typename... Component>
  static void CopyComponent(ComponentGroup<Component...>,
                            entt::registry& dst,
                            entt::registry& src,
                            const std::unordered_map<UUID, entt::entity>& entt_map) {
    CopyComponent<Component...>(dst, src, entt_map);
  }
  
  template<typename... Component>
  static void CopyComponentIfExists(Entity dst, Entity src) {
    ([&]()
     {
      if (src.HasComponent<Component>())
        dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
    }(), ...);
  }
  
  template<typename... Component>
  static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src) {
    CopyComponentIfExists<Component...>(dst, src);
  }
  
  template<typename Component>
  static void CopySingleComponentIfExists(Entity& dst, Entity& src) {
    if (src.HasComponent<Component>())
      dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
  }
  
  std::shared_ptr<EnttScene> EnttScene::Copy(std::shared_ptr<EnttScene> other) {
    std::shared_ptr<EnttScene> new_scene = std::make_shared<EnttScene>(other->file_path_);
    
    new_scene->setting_= other->setting_;
    
    new_scene->viewport_width_ = other->viewport_width_;
    new_scene->viewport_height_ = other->viewport_width_;
    
    auto& src_scene_registry = other->registry_;
    auto& dst_scene_registry = new_scene->registry_;
    std::unordered_map<UUID, entt::entity> entt_map;
    
    // Create entities in new scene
    auto id_view = src_scene_registry.view<IDComponent>();
    for (auto e : id_view) {
      UUID uuid = src_scene_registry.get<IDComponent>(e).id;
      const auto& name = src_scene_registry.get<TagComponent>(e).tag;
      Entity new_entity = new_scene->CreateEntity(name, uuid);
      entt_map[uuid] = (entt::entity)new_entity;
    }
    
    // Copy components (except IDComponent and TagComponent)
    CopyComponent(AllComponents{}, dst_scene_registry, src_scene_registry, entt_map);
    
    return new_scene;
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
    Entity entity = CreateNewEmptyEntity(uuid);
    
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
    
    // Delete
    if (physics_world_ and entity.HasComponent<RigidBodyComponent>()) {
      auto& rb = entity.GetComponent<RigidBodyComponent>();
      physics_world_->DestroyBody(rb.runtime_body);
      rb.runtime_body = nullptr;
    }
    
    // Delete the eneity from the map
    entity_id_map_.erase(entity);
    registry_.destroy(entity);
  }
  
  Entity EnttScene::DuplicateEntity(Entity entity) {
    Entity new_entity = CreateNewEmptyEntity(UUID());
    CopySingleComponentIfExists<TagComponent>(new_entity, entity);
    
    CopyComponentIfExists(AllComponents{}, new_entity, entity);
    
    return new_entity;
  }
  
  Entity EnttScene::CreateNewEmptyEntity(UUID uuid) {
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
    update_(ts);
  }
  
  void EnttScene::UpdateEditor(Timestep ts) {
    if (setting_.use_editor_camera) {
      editor_camera_.Update(ts);
      Render2DEntities(editor_camera_.GetViewProjection());
    } else {
      UpdateRuntime(ts);
    }
  }
  
  void EnttScene::UpdateRuntime(Timestep ts) {
    if (state_ == State::Play) { // TODO: Temp check till we use scene camera in editor mode
      // Update scripts
      InstantiateScript(ts);
      
      // Physics
      const int32_t velocity_iteration = 6;
      const int32_t position_iteration = 2;
      
      physics_world_->Step(ts, velocity_iteration, position_iteration);
      
      // Get Transform
      auto view = registry_.view<RigidBodyComponent>();
      for (auto e : view) {
        Entity& entity = entity_id_map_.at(e);
        
        auto& rb2d = entity.GetComponent<RigidBodyComponent>();
        if (rb2d.type == b2_dynamicBody or rb2d.type == b2_kinematicBody) {
          auto& transform = entity.GetComponent<TransformComponent>();
          b2Body* body = (b2Body*)rb2d.runtime_body;
          if (body != nullptr) {
            const auto& position = body->GetPosition();
            
            transform.UpdateRotation_Z(body->GetAngle());
            transform.UpdateTranslation({position.x, position.y, transform.Translation().z});
          }
        }
      }
    }
    
    if (primary_camera_data_.scene_camera) {
      Render2DEntities(primary_camera_data_.scene_camera->GetProjection() * glm::inverse(primary_camera_data_.transform_matrix));
    }
  }
  
  void EnttScene::AddBodyToPhysicsWorld(Entity entity, RigidBodyComponent& rb2d) {
    auto& transform = entity.GetComponent<TransformComponent>();
    
    b2BodyDef body_def;
    body_def.type = rb2d.type;
    body_def.position.Set(transform.Translation().x, transform.Translation().y);
    body_def.angle = transform.Rotation().z;
    
    body_def.linearVelocity = {rb2d.velocity.x, rb2d.velocity.y};
    body_def.angularVelocity = rb2d.angular_velocity;
    body_def.linearDamping = rb2d.linear_damping;
    body_def.angularDamping = rb2d.angular_damping;
    body_def.gravityScale = rb2d.gravity_scale;
    
    //      body_def.userData.pointer;
    
    b2Body* body = physics_world_->CreateBody(&body_def);
    body->SetFixedRotation(rb2d.fixed_rotation);
    rb2d.runtime_body = body;
    
    if (entity.HasComponent<BoxColliderComponent>()) {
      auto& bc2d = entity.GetComponent<BoxColliderComponent>();
      AddBoxColliderData(transform, bc2d, rb2d);
    }
    
    if (entity.HasComponent<CircleColliiderComponent>()) {
      auto& cc2d = entity.GetComponent<CircleColliiderComponent>();
      AddCircleColliderData(transform, cc2d, rb2d);
    }
    
    if (entity.HasComponent<PillBoxColliderComponent>()) {
      auto& pbc = entity.GetComponent<PillBoxColliderComponent>();
      AddPillColliderData(transform, pbc, rb2d);
    }
  }
  
  bool EnttScene::CheckOnGround(Entity* entity, float width, float height) {
    const glm::vec2& position = glm::vec2(entity->GetComponent<TransformComponent>().Translation());
    glm::vec2 ray_cast_1_begin = position;
    ray_cast_1_begin -= glm::vec2(width / 2.0f, 0.0f);
    
    glm::vec2 ray_cast_1_end = ray_cast_1_begin + glm::vec2(0.0f, height);
    std::shared_ptr<RayCastInfo> info_1 = RayCast(entity,
                                                  {ray_cast_1_begin.x, ray_cast_1_begin.y},
                                                  {ray_cast_1_end.x, ray_cast_1_end.y});
    
    
    glm::vec2 ray_cast_2_begin = ray_cast_1_begin + glm::vec2(width, 0.0f);
    glm::vec2 ray_cast_2_end = ray_cast_1_end + glm::vec2(width, 0.0f);
    
    std::shared_ptr<RayCastInfo> info_2 = RayCast(entity,
                                                  {ray_cast_2_begin.x, ray_cast_2_begin.y},
                                                  {ray_cast_2_end.x, ray_cast_2_end.y});
    
    bool on_ground = info_1->OnGround() or info_2->OnGround();

#if DEBUG_DRAW
      const auto& cd = primary_camera_data_;
      BatchRenderer::BeginBatch(cd.scene_camera->GetProjection() * glm::inverse(cd.transform_matrix));
      BatchRenderer::DrawLine(glm::vec3(ray_cast_1_begin, 0.0f), glm::vec3(ray_cast_1_end, 0.0f), {0, 1, 0, 1});
      BatchRenderer::DrawLine(glm::vec3(ray_cast_2_begin, 0.0f), glm::vec3(ray_cast_2_end, 0.0f), {0, 1, 0, 1});
      BatchRenderer::EndBatch();
#endif
    return on_ground;

  }
  
  void EnttScene::RuntimeStart() {
    physics_world_ = new b2World({0.0f, -9.8f});
    if (client_listner_)
      physics_world_->SetContactListener(client_listner_);
    
    auto view = registry_.view<RigidBodyComponent>();
    for (auto e : view) {
      Entity entity = { e, this };
      auto& rb2d = entity.GetComponent<RigidBodyComponent>();
      AddBodyToPhysicsWorld(entity,rb2d);
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
    if (setting_.use_editor_camera)
      editor_camera_.RendererGui(&setting_.editor_camera);
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
      if (!nsc.script) {
        ScriptManager::UpdateScript(&nsc, nsc.script_name, nsc.loader_function);
      }
      else {
        if (!nsc.script->scene_) {
          nsc.script->scene_ = this;
          nsc.script->Create(Entity{ entity, this });
        }
        
        nsc.script->Update(ts);
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
    } // for (const auto& entity : circle_view)
    
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
    } // for (const auto& entity : quad_view)
    
    auto sprite_view = registry_.view<TransformComponent, SpriteComponent>();
    // For all sprite entity
    for (const auto& sprite_entity : sprite_view) {
      const auto& [transform_component, sprite_component] = sprite_view.get<TransformComponent, SpriteComponent>(sprite_entity);
      BatchRenderer::DrawQuad(transform_component.GetTransform(),
                              sprite_component.sub_texture,
                              glm::vec4(1.0f),
                              (uint32_t)sprite_entity);
    } // for (const auto& entity : sprite_view)
    BatchRenderer::EndBatch();
  }
  
  std::shared_ptr<RayCastInfo> EnttScene::RayCast(Entity* requesting_obj, const glm::vec2& hit_point, const glm::vec2& normal) {
    std::shared_ptr<RayCastInfo> callback = std::make_shared<RayCastInfo>(requesting_obj);
    physics_world_->RayCast(callback.get(), { hit_point.x, hit_point.y }, { normal.x, normal.y });
    return callback;
  }
  
  void EnttScene::SetFilePath(const std::string& file_path) {
    file_path_ = file_path;
    name_ = StringUtils::GetNameFromFilePath(file_path_);
  }
  
  void EnttScene::SetSelectedEntity(Entity* entity) { selected_entity_ = entity; }
  Entity* EnttScene::GetSelectedEntity() { return selected_entity_; }
  
  uint32_t EnttScene::GetNumEntities() const { return num_entities_; }
  uint32_t EnttScene::GetMaxEntityId() const { return max_entity_id_; }
  
  EnttScene::Setting& EnttScene::GetSetting() { return setting_; }
  EditorCamera* EnttScene::GetEditorCamera() { return &editor_camera_; }
  bool EnttScene::IsEditing() const { return state_ == EnttScene::State::Edit; }
  const std::string& EnttScene::GetName() const { return name_; }
  const std::string& EnttScene::GetFilePath() const { return file_path_; }
  const CameraData& EnttScene::GetPrimaryCameraData() const { return primary_camera_data_; }
  bool EnttScene::UseEditorCamera() const { return setting_.use_editor_camera; }
  
  b2World* EnttScene::GetPhysicsWorld() { return physics_world_; }
  void EnttScene::SetContactListner(b2ContactListener *listener) {
    IK_CORE_ASSERT(listener);
    client_listner_ = listener;
  }
  
  void EnttScene::AddBoxColliderData(const TransformComponent& tc,
                                     const BoxColliderComponent& bc2d,
                                     RigidBodyComponent& rb2d,
                                     bool is_pill) {
    b2Body* body = rb2d.runtime_body;
    b2PolygonShape polygon_shape;
    if (is_pill)
      polygon_shape.SetAsBox(bc2d.size.x, bc2d.size.y, {bc2d.offset.x, bc2d.offset.y}, 0);
    else
      polygon_shape.SetAsBox(bc2d.size.x * tc.Scale().x, bc2d.size.y * tc.Scale().y, {bc2d.offset.x, bc2d.offset.y}, 0);
    
    b2FixtureDef fixture_def;
    fixture_def.shape = & polygon_shape;
    fixture_def.density = bc2d.density;
    fixture_def.friction = bc2d.friction;
    fixture_def.restitution = bc2d.restitution;
    fixture_def.restitutionThreshold = bc2d.restitution_threshold;
    fixture_def.isSensor = rb2d.is_sensor;
    fixture_def.userData.pointer = (uintptr_t)bc2d.runtime_fixture;
    
    body->CreateFixture(&fixture_def);
  }
  
  void EnttScene::AddCircleColliderData(const TransformComponent& tc,
                                        const CircleColliiderComponent& cc2d,
                                        RigidBodyComponent& rb2d,
                                        bool is_pill) {
    b2Body* body = rb2d.runtime_body;
    b2CircleShape circle_shape;
    circle_shape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
    if (is_pill)
      circle_shape.m_radius = cc2d.radius;
    else
      circle_shape.m_radius = std::abs(tc.Scale().x) * cc2d.radius;
    
    b2FixtureDef fixture_def;
    fixture_def.shape = & circle_shape;
    fixture_def.density = cc2d.density;
    fixture_def.friction = cc2d.friction;
    fixture_def.restitution = cc2d.restitution;
    fixture_def.restitutionThreshold = cc2d.restitution_threshold;
    fixture_def.isSensor = rb2d.is_sensor;
    fixture_def.userData.pointer = (uintptr_t)cc2d.runtime_fixture;
    
    body->CreateFixture(&fixture_def);
  }
  
  void EnttScene::AddPillColliderData(const TransformComponent &tc,
                                      const PillBoxColliderComponent &pbc,
                                      RigidBodyComponent& rb2d) {
    AddBoxColliderData(tc, pbc.bcc, rb2d, true);
    AddCircleColliderData(tc, pbc.top_ccc, rb2d, true);
    AddCircleColliderData(tc, pbc.bottom_ccc, rb2d, true);
  }
  
  int32_t FixtureListSize(b2Body* body) {
    int32_t size = 0;
    b2Fixture* fixture = body->GetFixtureList();
    while (fixture) {
      size++;
      fixture = fixture->GetNext();
    }
    return size;
  }
  
  void EnttScene::ResetPillBoxColliderData(const TransformComponent &tc, RigidBodyComponent* rb, const PillBoxColliderComponent &pbc) {
    b2Body* body = rb->runtime_body;
    int32_t size = FixtureListSize(body);
    for (int32_t i = 0; i < size; i++) {
      body->DestroyFixture(body->GetFixtureList());
    }
    
    AddPillColliderData(tc, pbc, *rb);
    body->ResetMassData();
  }
  
  float RayCastInfo::ReportFixture(b2Fixture *fixture, const b2Vec2 &hit_point, const b2Vec2 &normal, float fraction) {
    if ((Entity*)fixture->GetUserData().pointer == request_object) {
      return 1;
    }
    
    this->fixture = fixture;
    this->hit_point = hit_point;
    this->normal = normal;
    this->hit = true;
    this->fraction = fraction;    
    this->hit_object = (Entity*)fixture->GetUserData().pointer;
    
    return fraction;
  }
  
  bool RayCastInfo::OnGround() {
    return hit and hit_object and hit_object->HasComponent<RigidBodyComponent>() and hit_object->GetComponent<RigidBodyComponent>().is_ground;
  }

}

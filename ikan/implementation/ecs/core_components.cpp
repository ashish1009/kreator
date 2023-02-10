//
//  core_components.cpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#include "core_components.hpp"
#include "editor/property_grid.hpp"
#include "ecs/scriptable_entity.hpp"

namespace ikan {
  
  // -------------------------------------------------------------------------
  // ID Component
  // -------------------------------------------------------------------------
  IDComponent::IDComponent(const UUID& id) : id(id) {
    IK_CORE_TRACE(LogModule::Component, "Creating ID Component ...");
  }
  IDComponent::IDComponent(const IDComponent& other)
  : id(other.id) {
    IK_CORE_TRACE(LogModule::Component, "Copying ID Component ...");
  }
  IDComponent::IDComponent(IDComponent&& other)
  : id(other.id) {
    IK_CORE_TRACE(LogModule::Component, "Moving ID Component ...");
  }
  IDComponent& IDComponent::operator=(const IDComponent& other) {
    IK_CORE_TRACE(LogModule::Component, "Copying ID Component using = operator ...");
    id = other.id;
    return *this;
  }
  IDComponent& IDComponent::operator=(IDComponent&& other) {
    IK_CORE_TRACE(LogModule::Component, "Moving ID Component using = operator ...");
    id = other.id;
    return *this;
  }
  
  // -------------------------------------------------------------------------
  // Tag Component
  // -------------------------------------------------------------------------
  TagComponent::TagComponent(const std::string& tag)
  : tag(tag) {
    IK_CORE_TRACE(LogModule::Component, "Creating Tag Component ...");
  }
  TagComponent::TagComponent(const TagComponent& other)
  : tag(other.tag) {
    IK_CORE_TRACE(LogModule::Component, "Copying Tag Component ...");
  }
  TagComponent::TagComponent(TagComponent&& other)
  : tag(other.tag) {
    IK_CORE_TRACE(LogModule::Component, "Moving Tag Ccomponent ...");
  }
  TagComponent& TagComponent::operator=(const TagComponent& other) {
    IK_CORE_TRACE(LogModule::Component, "Copying Tag Component using = operator...");
    tag = other.tag;
    return *this;
  }
  TagComponent& TagComponent::operator=(TagComponent&& other) {
    IK_CORE_TRACE(LogModule::Component, "Moving Tag Component using = operator ...");
    tag = other.tag;
    return *this;
  }

  // -------------------------------------------------------------------------
  // Transform Component
  // -------------------------------------------------------------------------
  TransformComponent::TransformComponent(const glm::vec3& translation)
  : translation(translation) {
    IK_CORE_TRACE(LogModule::Component, "Creating Transform Component ...");
    transform = Math::GetTransformMatrix(translation, rotation, scale);
  }
  TransformComponent::TransformComponent(const TransformComponent& other)
  : translation(other.Translation()), scale(other.Scale()), rotation(other.Rotation()) {
    IK_CORE_TRACE(LogModule::Component, "Copying Transform Component ...");
    transform = Math::GetTransformMatrix(translation, rotation, scale);
  }
  TransformComponent::TransformComponent(TransformComponent&& other)
  : translation(other.Translation()), scale(other.Scale()), rotation(other.Rotation()) {
    IK_CORE_TRACE(LogModule::Component, "Moving Transform Component ...");
    transform = Math::GetTransformMatrix(translation, rotation, scale);
  }
  TransformComponent& TransformComponent::operator=(const TransformComponent& other) {
    translation = other.Translation();
    scale = other.Scale();
    rotation = other.Rotation();
    transform = Math::GetTransformMatrix(translation, rotation, scale);
    IK_CORE_TRACE(LogModule::Component, "Copying Transform Component using = operator...");
    return *this;
  }
  TransformComponent& TransformComponent::operator=(TransformComponent&& other) {
    translation = other.Translation();
    scale = other.Scale();
    rotation = other.Rotation();
    transform = Math::GetTransformMatrix(translation, rotation, scale);
    IK_CORE_TRACE(LogModule::Component, "Moving Transform Component using = operator...");
    return *this;
  }

  const glm::mat4& TransformComponent::GetTransform() const { return transform; }
  const glm::vec3& TransformComponent::Translation() const { return translation; }
  const glm::vec3& TransformComponent::Rotation() const { return rotation; }
  const glm::vec3& TransformComponent::Scale() const { return scale; }
  
  void TransformComponent::UpdateTranslation_X(float value) { UpdateTranslation({value, translation.y, translation.z} ); }
  void TransformComponent::UpdateRotation_X(float value) { UpdateRotation({value, rotation.y, rotation.z} ); }
  void TransformComponent::UpdateScale_X(float value) { UpdateScale({value, scale.y, scale.z} ); }
  
  void TransformComponent::UpdateTranslation_Y(float value) { UpdateTranslation({translation.x, value, translation.z} ); }
  void TransformComponent::UpdateRotation_Y(float value) { UpdateRotation({rotation.x, value, rotation.z} ); }
  void TransformComponent::UpdateScale_Y(float value) { UpdateScale({scale.x, value, scale.z} ); }
  
  void TransformComponent::UpdateTranslation_Z(float value) { UpdateTranslation({translation.x, translation.y, value} ); }
  void TransformComponent::UpdateRotation_Z(float value) { UpdateRotation({rotation.x, rotation.y, value} ); }
  void TransformComponent::UpdateScale_Z(float value) { UpdateScale({scale.x, scale.y, value} ); }

  void TransformComponent::UpdateTranslation(const glm::vec3& value) {
    translation = value;
    transform = Math::GetTransformMatrix(translation, rotation, scale);
  }
  void TransformComponent::UpdateRotation(const glm::vec3& value) {
    rotation = value;
    transform = Math::GetTransformMatrix(translation, rotation, scale);
  }
  void TransformComponent::UpdateScale(const glm::vec3& value) {
    scale = value;
    transform = Math::GetTransformMatrix(translation, rotation, scale);
  }

  void TransformComponent::RenderGui() {
    if (PropertyGrid::Float3("Translation", translation, nullptr, 0.25f, 0.0f, 80.0f)) {
      transform = Math::GetTransformMatrix(translation, rotation, scale);
    }
    
    glm::vec3 rotation_in_degree = glm::degrees(rotation);
    if (PropertyGrid::Float3("Rotation", rotation_in_degree, nullptr, 0.25f, 0.0f, 80.0f)) {
      rotation = glm::radians(rotation_in_degree);
      transform = Math::GetTransformMatrix(translation, rotation, scale);
    }
    
    if (PropertyGrid::Float3("Scale", scale, nullptr, 0.25f, 1.0f, 80.0f)) {
      transform = Math::GetTransformMatrix(translation, rotation, scale);
    }
    ImGui::Separator();
  }

  // -------------------------------------------------------------------------
  // Quad Component
  // -------------------------------------------------------------------------
  QuadComponent::QuadComponent() {
    IK_CORE_TRACE(LogModule::Component, "Creating Quad Component ...");
  }
  QuadComponent::QuadComponent(const QuadComponent& other)
  : color(other.color) {
    texture_comp = other.texture_comp;
    IK_CORE_TRACE(LogModule::Component, "Copying Quad Component ...");
  }
  QuadComponent::QuadComponent(QuadComponent&& other)
  : color(other.color) {
    texture_comp = other.texture_comp;
    IK_CORE_TRACE(LogModule::Component, "Moving Quad Component ...");
  }
  QuadComponent& QuadComponent::operator=(const QuadComponent& other) {
    color = other.color;
    texture_comp = other.texture_comp;
    IK_CORE_TRACE(LogModule::Component, "Copying Quad Component using = operator...");
    return *this;
  }
  QuadComponent& QuadComponent::operator=(QuadComponent&& other) {
    color = other.color;
    texture_comp = other.texture_comp;
    IK_CORE_TRACE(LogModule::Component, "Moving Quad Component using = operator...");
    return *this;
  }
  
  void QuadComponent::RenderGui() {
    texture_comp.RenderGui(color, [this]() {
      ImGui::ColorEdit4("Color ", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    });
    
    if (texture_comp.sprite and texture_comp.use_sprite) {
      // Subtexture property
      glm::vec2 coords = texture_comp.sprite->GetCoords();
      glm::vec2 sprite_size = texture_comp.sprite->GetSpriteSize();
      glm::vec2 cell_size   = texture_comp.sprite->GetCellSize();

      if (PropertyGrid::Float2("Coords", coords, nullptr, 0.1f, 0.0f, 100.0f, 0.0f)) {
        texture_comp.sprite->GetSpriteImage().reset();
        texture_comp.sprite = SubTexture::CreateFromCoords(texture_comp.component, coords, sprite_size, cell_size);
      }
      if (PropertyGrid::Float2("Sprite Size", sprite_size, nullptr, 1.0f, 1.0f, 100.0f, 8.0f)) {
        texture_comp.sprite->GetSpriteImage().reset();
        texture_comp.sprite = SubTexture::CreateFromCoords(texture_comp.component, coords, sprite_size, cell_size);
      }
      if (PropertyGrid::Float2("Cell Size", cell_size, nullptr, 8.0f, 16.0f, 100.0f, 1.0f)) {
        texture_comp.sprite->GetSpriteImage().reset();
        texture_comp.sprite = SubTexture::CreateFromCoords(texture_comp.component, coords, sprite_size, cell_size);
      }
      ImGui::Separator();
      
      const ImGuiTreeNodeFlags tree_node_flags =
      ImGuiTreeNodeFlags_SpanAvailWidth |
      ImGuiTreeNodeFlags_AllowItemOverlap |
      ImGuiTreeNodeFlags_DefaultOpen;
      
      // Render the title named as entity name
      bool open = ImGui::TreeNodeEx("Sprite", tree_node_flags);
      if (open) {
        size_t tex_id = texture_comp.sprite->GetSpriteImage()->GetRendererID();
        
        float tex_width = (float)texture_comp.sprite->GetSpriteImage()->GetWidth();
        float tex_height = (float)texture_comp.sprite->GetSpriteImage()->GetHeight() ;
        float width = std::min(ImGui::GetContentRegionAvailWidth(), tex_width);
        
        float size_ratio = width / tex_width;
        float height = tex_height * size_ratio;
        
        ImGui::Image((void*)tex_id, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0),
                     ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
        
        
        ImVec2 pos = ImGui::GetCursorScreenPos();
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          
          ImGuiIO& io = ImGui::GetIO();
          
          float region_fixed_x = (float)((int32_t)(sprite_size.x * cell_size.x * size_ratio));
          float region_fixed_y = (float)((int32_t)(sprite_size.y * cell_size.y * size_ratio));
          static float zoom = 10.0f;
          
          float region_x = io.MousePos.x - pos.x - region_fixed_x * 0.5f;
          if (region_x < 0.0f)
            region_x = 0.0f;
          
          else if (region_x > width - region_fixed_x)
            region_x = width - region_fixed_x;
          
          float region_y = pos.y - io.MousePos.y - region_fixed_y * 0.5f;
          if (region_y < 0.0f)
            region_y = 0.0f;
          
          else if (region_y > width - region_fixed_y)
            region_y = height - region_fixed_y;
          
          ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
          ImGui::Text("Max: (%.2f, %.2f)", region_x + region_fixed_x, region_y + region_fixed_y);
          
          ImVec2 uv0 = ImVec2((region_x) / width, (region_y + region_fixed_y) / height);
          ImVec2 uv1 = ImVec2((region_x + region_fixed_x) / width, (region_y) / height);
          
          ImGui::Image((void*)tex_id, ImVec2(region_fixed_x * zoom, region_fixed_y * zoom),
                       uv0, uv1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
          
          if (ImGui::IsMouseClicked(0)) {
            glm::vec3 coords;
            coords.x = (((region_x + region_fixed_x)) / (cell_size.x * size_ratio)) - sprite_size.x;
            coords.y = (((region_y + region_fixed_y)) / (cell_size.y * size_ratio)) - sprite_size.y;
            
            texture_comp.sprite->GetSpriteImage().reset();
            texture_comp.sprite = SubTexture::CreateFromCoords(texture_comp.component, coords, sprite_size, cell_size);
          }
          
          ImGui::EndTooltip();
        }
        ImGui::TreePop();
      }
    }
  }
  
  // -------------------------------------------------------------------------
  // Sprite Component
  // -------------------------------------------------------------------------
  SpriteComponent::SpriteComponent(std::shared_ptr<SubTexture> sub_tex)
  : sub_texture(sub_tex) {
    IK_CORE_TRACE(LogModule::Component, "Creating Sprite Component ...");
  }
  SpriteComponent::SpriteComponent(const SpriteComponent& other) {
    if (other.sub_texture)
      sub_texture = other.sub_texture;
    IK_CORE_TRACE(LogModule::Component, "Copying Sprite Component ...");
  }
  SpriteComponent::SpriteComponent(SpriteComponent&& other) {
    if (other.sub_texture)
      sub_texture = other.sub_texture;
    IK_CORE_TRACE(LogModule::Component, "Moving Sprite Component ...");
  }
  SpriteComponent& SpriteComponent::operator=(const SpriteComponent& other) {
    if (other.sub_texture)
      sub_texture = other.sub_texture;
    IK_CORE_TRACE(LogModule::Component, "Copying Sprite Component using = operator...");
    return *this;
  }
  SpriteComponent& SpriteComponent::operator=(SpriteComponent&& other) {
    if (other.sub_texture)
      sub_texture = other.sub_texture;
    IK_CORE_TRACE(LogModule::Component, "Moving Sprite Component using = operator...");
    return *this;
  }
  
  void SpriteComponent::RenderGui() {
    
  }
  
  // -------------------------------------------------------------------------
  // Circle Component
  // -------------------------------------------------------------------------
  CircleComponent::CircleComponent() {
    IK_CORE_TRACE(LogModule::Component, "Creating Circle Component ...");
  }
  CircleComponent::CircleComponent(const CircleComponent& other)
  : color(other.color), thickness(other.thickness), fade(other.fade) {
    if (other.texture_comp.component) {
      texture_comp.use = other.texture_comp.use;
      texture_comp.tiling_factor = other.texture_comp.tiling_factor;
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    }
    IK_CORE_TRACE(LogModule::Component, "Copying Circle Component ...");
  }
  CircleComponent::CircleComponent(CircleComponent&& other)
  : color(other.color), thickness(other.thickness), fade(other.fade) {
    if (other.texture_comp.component) {
      texture_comp.use = other.texture_comp.use;
      texture_comp.tiling_factor = other.texture_comp.tiling_factor;
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    }
    IK_CORE_TRACE(LogModule::Component, "Moving Circle Component ...");
  }
  CircleComponent& CircleComponent::operator=(const CircleComponent& other) {
    color = other.color;
    thickness = other.thickness;
    fade = other.fade;
    if (other.texture_comp.component) {
      texture_comp.use = other.texture_comp.use;
      texture_comp.tiling_factor = other.texture_comp.tiling_factor;
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    }
    IK_CORE_TRACE(LogModule::Component, "Copying Circle Component using = operator...");
    return *this;
  }
  CircleComponent& CircleComponent::operator=(CircleComponent&& other) {
    color = other.color;
    thickness = other.thickness;
    fade = other.fade;
    if (other.texture_comp.component) {
      texture_comp.use = other.texture_comp.use;
      texture_comp.tiling_factor = other.texture_comp.tiling_factor;
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    }
    IK_CORE_TRACE(LogModule::Component, "Moving Quad Component using = operator...");
    return *this;
  }

  void CircleComponent::RenderGui() {
    texture_comp.RenderGui(color, [this]() {
      ImGui::ColorEdit4("Color ", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    });
    
    PropertyGrid::Float1("Thickness", thickness, nullptr, 0.05f, 1.0f, 100.0f, 0.05, 1.0f);
    PropertyGrid::Float1("Fade", fade, nullptr, 0.1f, 0.0f, 100.0f, 0.0f);
    ImGui::Separator();
  }

  // -------------------------------------------------------------------------
  // Camera Component
  // -------------------------------------------------------------------------
  CameraComponent::CameraComponent(const CameraComponent& other)
  : is_primary(other.is_primary), is_fixed_aspect_ratio(other.is_fixed_aspect_ratio) {
    camera = std::make_shared<SceneCamera>();
    *(camera.get()) = *(other.camera.get());
  }
  CameraComponent::CameraComponent(CameraComponent&& other)
  : is_primary(other.is_primary), is_fixed_aspect_ratio(other.is_fixed_aspect_ratio) {
    camera = std::make_shared<SceneCamera>();
    *(camera.get()) = *(other.camera.get());
  }
  CameraComponent& CameraComponent::operator=(const CameraComponent& other) {
    is_primary = other.is_primary;
    is_fixed_aspect_ratio = other.is_fixed_aspect_ratio;
    camera = std::make_shared<SceneCamera>();
    *(camera.get()) = *(other.camera.get());
    return *this;
  }
  CameraComponent& CameraComponent::operator=(CameraComponent&& other) {
    is_primary = other.is_primary;
    is_fixed_aspect_ratio = other.is_fixed_aspect_ratio;
    camera = std::make_shared<SceneCamera>();
    *(camera.get()) = *(other.camera.get());
    camera = std::move(other.camera);
    return *this;
  }
  CameraComponent::CameraComponent(SceneCamera::ProjectionType proj_type) {
    camera = std::make_shared<SceneCamera>(proj_type);
  }
  void CameraComponent::RenderGui() {
    auto column_width = ImGui::GetWindowContentRegionMax().x / 2;
    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, column_width);
    ImGui::Checkbox("Primary", &is_primary); ImGui::SameLine();
    
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, column_width);
    ImGui::Checkbox("Fixed Aspect Ratio", &is_fixed_aspect_ratio);
    
    ImGui::Columns(1);
    
    camera->RenderGui();
    ImGui::Separator();
  }
  
  // -------------------------------------------------------------------------
  // Native Script Component
  // -------------------------------------------------------------------------
  NativeScriptComponent::NativeScriptComponent(ScriptLoaderFn loader_fun)
  : loader_function(loader_fun) {
  }
  
  NativeScriptComponent::NativeScriptComponent(const NativeScriptComponent& other) {
    loader_function = other.loader_function;

    for (const auto& [name, script] : other.scrip_name_map) {
      scrip_name_map[name] = script;
      ScriptManager::UpdateScript(this, name, loader_function);
    }
  }
  NativeScriptComponent& NativeScriptComponent::operator=(const NativeScriptComponent& other) {
    loader_function = other.loader_function;
    
    for (const auto& [name, script] : other.scrip_name_map) {
      scrip_name_map[name] = script;
      ScriptManager::UpdateScript(this, name, loader_function);
    }
    return *this;
  }
  NativeScriptComponent::NativeScriptComponent(NativeScriptComponent&& other) {
    loader_function = other.loader_function;
    
    for (const auto& [name, script] : other.scrip_name_map) {
      scrip_name_map[name] = script;
      ScriptManager::UpdateScript(this, name, loader_function);
    }
  }
  NativeScriptComponent& NativeScriptComponent::operator=(NativeScriptComponent&& other) {
    loader_function = other.loader_function;
    
    for (const auto& [name, script] : other.scrip_name_map) {
      scrip_name_map[name] = script;
      ScriptManager::UpdateScript(this, name, loader_function);
    }
    return *this;
  }
  
  NativeScriptComponent::~NativeScriptComponent() {
    for (const auto& [name, script] : scrip_name_map) {
      delete script;
    }
    scrip_name_map.clear();
  }
  
  void NativeScriptComponent::RenderGui() {
    ImGui::PushID("Natiove Script Component");
    
    const ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_SpanAvailWidth |
    ImGuiTreeNodeFlags_AllowItemOverlap |
    ImGuiTreeNodeFlags_DefaultOpen |
    ImGuiTreeNodeFlags_FramePadding;
    
    // Render the title named as entity name
    bool open = ImGui::TreeNodeEx("Scripts", tree_node_flags);
    if (open) {
      for (const auto& [name, script] : scrip_name_map) {
        bool opened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Bullet);
        if (opened) {
          script->RenderGui();
          ImGui::TreePop();
        }
      }
      ImGui::TreePop();
    }
    ImGui::Separator();
    
    int32_t new_script_idx = -1;
    new_script_idx = PropertyGrid::ComboDrop("Add Script",
                                             ScriptManager::scripts_,
                                             0,
                                             ImGui::GetWindowContentRegionMax().x / 2);
    if (new_script_idx > 0) {
      const auto script_name = ScriptManager::scripts_[new_script_idx];
      if (scrip_name_map.find(script_name) == scrip_name_map.end())
        ScriptManager::UpdateScript(this, script_name, nullptr);
      else
        IK_CORE_TRACE(LogModule::Component, "Script {0} already loaded to current entity", script_name.c_str());
    }
    
    ImGui::PopID();
  }
  
  // -------------------------------------------------------------------------
  // Rigid Body Component
  // -------------------------------------------------------------------------
  RigidBodyComponent::RigidBodyComponent(const RigidBodyComponent& other) {
    type = other.type;
    fixed_rotation = other.fixed_rotation;
  }
  RigidBodyComponent& RigidBodyComponent::operator=(const RigidBodyComponent& other) {
    type = other.type;
    fixed_rotation = other.fixed_rotation;
    return *this;
  }
  RigidBodyComponent::RigidBodyComponent(RigidBodyComponent&& other) {
    type = other.type;
    fixed_rotation = other.fixed_rotation;
  }
  RigidBodyComponent& RigidBodyComponent::operator=(RigidBodyComponent&& other) {
    type = other.type;
    fixed_rotation = other.fixed_rotation;
    return *this;
  }
  void RigidBodyComponent::RenderGui() {
    b2BodyType new_body_type = b2BodyType(PropertyGrid::ComboDrop("Rigid Body Type",
                                                                  { "Static" , "Kinamatic", "Dynamic" },
                                                                  (uint32_t)type,
                                                                  ImGui::GetWindowContentRegionMax().x / 2));
    
    // Render the property based on the projection type of camera
    if (new_body_type != type)
      type = new_body_type;

    PropertyGrid::CheckBox("Fixed Rotation", fixed_rotation, ImGui::GetWindowContentRegionMax().x / 2);
    ImGui::Separator();
  }
  
  // -------------------------------------------------------------------------
  // Box Colloider Component
  // -------------------------------------------------------------------------
  BoxColloiderComponent::BoxColloiderComponent(const BoxColloiderComponent& other) {
    offset = other.offset;
    size = other.size;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
  }
  BoxColloiderComponent& BoxColloiderComponent::operator=(const BoxColloiderComponent& other) {
    offset = other.offset;
    size = other.size;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
    return *this;
  }
  BoxColloiderComponent::BoxColloiderComponent(BoxColloiderComponent&& other) {
    offset = other.offset;
    size = other.size;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
  }
  BoxColloiderComponent& BoxColloiderComponent::operator=(BoxColloiderComponent&& other) {
    offset = other.offset;
    size = other.size;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
    return *this;
  }
  void BoxColloiderComponent::RenderGui() {
    PropertyGrid::Float2("Offset", offset);
    PropertyGrid::Float2("Size", size);
    ImGui::Separator();
    
    PropertyGrid::Float1("Density", density);
    PropertyGrid::Float1("Friction", friction);
    PropertyGrid::Float1("Restitution", restitution);
    PropertyGrid::Float1("Restitution Threshold", restitution_threshold);
    
    ImGui::Separator();
  }
  
  // -------------------------------------------------------------------------
  // Circle Colloider Component
  // -------------------------------------------------------------------------
  CircleColloiderComponent::CircleColloiderComponent(const CircleColloiderComponent& other) {
    offset = other.offset;
    radius = other.radius;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
  }
  CircleColloiderComponent& CircleColloiderComponent::operator=(const CircleColloiderComponent& other) {
    offset = other.offset;
    radius = other.radius;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
    return *this;
  }
  CircleColloiderComponent::CircleColloiderComponent(CircleColloiderComponent&& other) {
    offset = other.offset;
    radius = other.radius;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
  }
  CircleColloiderComponent& CircleColloiderComponent::operator=(CircleColloiderComponent&& other) {
    offset = other.offset;
    radius = other.radius;
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitution_threshold = other.restitution_threshold;
    return *this;
  }
  void CircleColloiderComponent::RenderGui() {
    PropertyGrid::Float2("Offset", offset);
    PropertyGrid::Float1("Radius", radius);
    ImGui::Separator();
    
    PropertyGrid::Float1("Density", density);
    PropertyGrid::Float1("Friction", friction);
    PropertyGrid::Float1("Restitution", restitution);
    PropertyGrid::Float1("Restitution Threshold", restitution_threshold);
    
    ImGui::Separator();
  }
  
  // -------------------------------------------------------------------------
  // Animation Component
  // -------------------------------------------------------------------------
  AnimationComponent::AnimationComponent(std::shared_ptr<Texture> sprite_image) {
    this->sprite_image = sprite_image;
  }

  AnimationComponent::AnimationComponent(const AnimationComponent& other) {
    animation = other.animation;
    is_sprite = other.is_sprite;
    
    sprite_image = Renderer::GetTexture(other.sprite_image->GetfilePath());
    for (const auto& sprite : other.sprites) {
      sprites.push_back(SubTexture::CreateFromCoords(sprite->GetSpriteImage(), sprite->GetCoords(),
                                                     sprite->GetSpriteSize(), sprite->GetCellSize()));
    }
  }
  AnimationComponent& AnimationComponent::operator=(const AnimationComponent& other) {
    animation = other.animation;
    is_sprite = other.is_sprite;
    
    sprite_image = Renderer::GetTexture(other.sprite_image->GetfilePath());
    for (const auto& sprite : other.sprites) {
      sprites.push_back(SubTexture::CreateFromCoords(sprite->GetSpriteImage(), sprite->GetCoords(),
                                                     sprite->GetSpriteSize(), sprite->GetCellSize()));
    }
    return *this;
  }
  AnimationComponent::AnimationComponent(AnimationComponent&& other) {
    animation = other.animation;
    is_sprite = other.is_sprite;
    
    sprite_image = Renderer::GetTexture(other.sprite_image->GetfilePath());
    for (const auto& sprite : other.sprites) {
      sprites.push_back(SubTexture::CreateFromCoords(sprite->GetSpriteImage(), sprite->GetCoords(),
                                                     sprite->GetSpriteSize(), sprite->GetCellSize()));
    }
  }
  AnimationComponent& AnimationComponent::operator=(AnimationComponent&& other) {
    animation = other.animation;
    is_sprite = other.is_sprite;
    
    sprite_image = Renderer::GetTexture(other.sprite_image->GetfilePath());
    for (const auto& sprite : other.sprites) {
      sprites.push_back(SubTexture::CreateFromCoords(sprite->GetSpriteImage(), sprite->GetCoords(),
                                                     sprite->GetSpriteSize(), sprite->GetCellSize()));
    }
    return *this;
  }
  void AnimationComponent::RenderGui() {
    PropertyGrid::CheckBox("Animation", animation);
    PropertyGrid::CheckBox("Is Sprite", is_sprite);
    ImGui::Separator();
    
    if (is_sprite) {
      static glm::vec2 coords = {0, 0}, sprite_size = {1, 1}, cell_size = {16, 16};
      PropertyGrid::Float2("Add Coord", coords, nullptr, 0.1f, 0.0f, 100.0f, 0.0f);
      PropertyGrid::Float2("Sprite Size", sprite_size, nullptr, 1.0f, 1.0f, 100.0f, 8.0f);
      PropertyGrid::Float2("Cell Size", cell_size, nullptr, 8.0f, 16.0f, 100.0f, 1.0f);
      
      ImGui::Separator();
      // Tag
      const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;
      
      // Render the title named as entity name
      bool open = ImGui::TreeNodeEx("Animation Sprites", tree_node_flags);
      
      // Get the avilable width and height for button position
      ImVec2 content_region_available = ImGui::GetContentRegionAvail();
      float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
      
      // Set the curson position on same line for (X) button
      ImGui::SameLine(content_region_available.x - line_height * 0.5f);
      float content_height = GImGui->Font->FontSize;
      
      const auto& current_cursor_pos = ImGui::GetCursorPos();
      ImGui::SetCursorPos({current_cursor_pos.x, current_cursor_pos.y + content_height / 4});
      
      // Render the button (X) for removing the component
      static std::shared_ptr<Texture> add_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/plus.png"));
      if (PropertyGrid::ImageButton("Add", add_texture->GetRendererID(), { content_height, content_height } )) {
        bool add = true;
        for (const auto& sprite : sprites) {
          if (sprite->GetCoords() == coords) {
            add = false;
            break;
          }
        }
        if (add)
          sprites.push_back(SubTexture::CreateFromCoords(sprite_image, coords, sprite_size, cell_size));
      }

      static bool delete_sprite = false;
      auto delete_it = sprites.begin();
      if (open) {
        for (auto it = sprites.begin(); it != sprites.end(); it++) {
          auto& sprite = *it;
          
          std::string sprite_data;
          sprite_data = std::to_string((int32_t)sprite->GetCoords().x) + " , " + std::to_string((int32_t)sprite->GetCoords().y) + " | ";
          sprite_data += std::to_string((int32_t)sprite->GetSpriteSize().x) + " , " + std::to_string((int32_t)sprite->GetSpriteSize().y) + " | ";
          sprite_data += std::to_string((int32_t)sprite->GetCellSize().x) + " , " + std::to_string((int32_t)sprite->GetCellSize().y);
          
          bool coord_open = ImGui::TreeNodeEx(sprite_data.c_str(), ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_OpenOnArrow);
          
          // Right click of mouse option
          if (ImGui::BeginPopupContextItem()) {
            // Delete Coord
            if (ImGui::MenuItem("Delete Coord")) {
              delete_sprite = true;
              delete_it = it;
            }
            ImGui::EndMenu();
          }

          if (coord_open)
            ImGui::TreePop();
        }
        ImGui::TreePop();
      }
      ImGui::Separator();
      
      if (delete_sprite) {
        sprites.erase(delete_it);
        delete_sprite = false;
      }
    }
  }
  
}

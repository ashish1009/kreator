//
//  core_components.cpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#include "core_components.hpp"
#include "editor/property_grid.hpp"

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
  }
  TransformComponent::TransformComponent(const TransformComponent& other)
  : translation(other.translation), scale(other.scale), rotation(other.rotation) {
    IK_CORE_TRACE(LogModule::Component, "Copying Transform Component ...");
  }
  TransformComponent::TransformComponent(TransformComponent&& other)
  : translation(other.translation), scale(other.scale), rotation(other.rotation) {
    IK_CORE_TRACE(LogModule::Component, "Moving Transform Component ...");
  }
  TransformComponent& TransformComponent::operator=(const TransformComponent& other) {
    translation = other.translation;
    scale = other.scale;
    rotation = other.rotation;
    IK_CORE_TRACE(LogModule::Component, "Copying Transform Component using = operator...");
    return *this;
  }
  TransformComponent& TransformComponent::operator=(TransformComponent&& other) {
    translation = other.translation;
    scale = other.scale;
    rotation = other.rotation;
    IK_CORE_TRACE(LogModule::Component, "Moving Transform Component using = operator...");
    return *this;
  }
  glm::mat4 TransformComponent::GetTransform() const {
    return Math::GetTransformMatrix(translation, rotation, scale);
  }
  
  void TransformComponent::RenderGui() {
    PropertyGrid::Float3("Translation", translation, nullptr, 0.25f, 0.0f, 80.0f);
    
    glm::vec3 rotation_in_degree = glm::degrees(rotation);
    PropertyGrid::Float3("Rotation", rotation_in_degree, nullptr, 0.25f, 0.0f, 80.0f);
    rotation = glm::radians(rotation_in_degree);
    
    PropertyGrid::Float3("Scale", scale, nullptr, 0.25f, 1.0f, 80.0f);
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
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Copying Quad Component ...");
  }
  QuadComponent::QuadComponent(QuadComponent&& other)
  : color(other.color) {
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Moving Quad Component ...");
  }
  QuadComponent& QuadComponent::operator=(const QuadComponent& other) {
    color = other.color;
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Copying Quad Component using = operator...");
    return *this;
  }
  QuadComponent& QuadComponent::operator=(QuadComponent&& other) {
    color = other.color;
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Moving Quad Component using = operator...");
    return *this;
  }
  
  void QuadComponent::RenderGui() {
    PropertyGrid::RenderTextureComponent(texture_comp, color, [this]() {
      ImGui::ColorEdit4("Color ", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    });
  }

  // -------------------------------------------------------------------------
  // Circle Component
  // -------------------------------------------------------------------------
  CircleComponent::CircleComponent() {
    IK_CORE_TRACE(LogModule::Component, "Creating Circle Component ...");
  }
  CircleComponent::CircleComponent(const CircleComponent& other)
  : color(other.color), thickness(other.thickness), fade(other.fade) {
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Copying Circle Component ...");
  }
  CircleComponent::CircleComponent(CircleComponent&& other)
  : color(other.color), thickness(other.thickness), fade(other.fade) {
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Moving Circle Component ...");
  }
  CircleComponent& CircleComponent::operator=(const CircleComponent& other) {
    color = other.color;
    thickness = other.thickness;
    fade = other.fade;
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Copying Circle Component using = operator...");
    return *this;
  }
  CircleComponent& CircleComponent::operator=(CircleComponent&& other) {
    color = other.color;
    thickness = other.thickness;
    fade = other.fade;
    if (other.texture_comp.component)
      texture_comp.component = Renderer::GetTexture(other.texture_comp.component->GetfilePath());
    IK_CORE_TRACE(LogModule::Component, "Moving Quad Component using = operator...");
    return *this;
  }

  void CircleComponent::RenderGui() {
    PropertyGrid::RenderTextureComponent(texture_comp, color, [this]() {
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
  : camera(std::move(other.camera)) {}
  CameraComponent& CameraComponent::operator=(const CameraComponent& other) {
    camera = other.camera;
    return *this;
  }
  CameraComponent& CameraComponent::operator=(CameraComponent&& other) {
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
    
    camera->RenderGui(true);
    ImGui::Separator();
  }
  
}

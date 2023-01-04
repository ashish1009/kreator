//
//  core_components.cpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#include "core_components.hpp"

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
    IK_CORE_TRACE(LogModule::Component, "Moving Tag Component using = operator...");
    return *this;
  }
  glm::mat4 TransformComponent::GetTransform() const {
    return Math::GetTransformMatrix(translation, rotation, scale);
  }
  
}

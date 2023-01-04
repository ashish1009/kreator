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
    IK_CORE_TRACE("Component", "Creating ID Ccomponent ...");
  }
  IDComponent::IDComponent(const IDComponent& other)
  : id(other.id) {
    IK_CORE_TRACE("Component", "Copying ID Ccomponent ...");
  }
  IDComponent::IDComponent(IDComponent&& other)
  : id(other.id) {
    IK_CORE_TRACE("Component", "Moving ID Ccomponent ...");
  }
  IDComponent& IDComponent::operator=(const IDComponent& other) {
    IK_CORE_TRACE("Component", "Copying ID Ccomponent using = operator ...");
    id = other.id;
    return *this;
  }
  IDComponent& IDComponent::operator=(IDComponent&& other) {
    IK_CORE_TRACE("Component", "Moving ID Ccomponent using = operator ...");
    id = other.id;
    return *this;
  }
  
  // -------------------------------------------------------------------------
  // Tag Component
  // -------------------------------------------------------------------------
  TagComponent::TagComponent(const std::string& tag)
  : tag(tag) {
    IK_CORE_TRACE("Component", "Creating Tag Ccomponent ...");
  }
  TagComponent::TagComponent(const TagComponent& other)
  : tag(other.tag) {
    IK_CORE_TRACE("Component", "Creating Tag Ccomponent ...");
  }
  TagComponent::TagComponent(TagComponent&& other)
  : tag(other.tag) {
    IK_CORE_TRACE("Component", "Creating Tag Ccomponent ...");
  }
  TagComponent& TagComponent::operator=(const TagComponent& other) {
    IK_CORE_TRACE("Component", "Creating Tag Ccomponent ...");
    tag = other.tag;
    return *this;
  }
  TagComponent& TagComponent::operator=(TagComponent&& other) {
    IK_CORE_TRACE("Component", "Creating Tag Ccomponent ...");
    tag = other.tag;
    return *this;
  }

  
}

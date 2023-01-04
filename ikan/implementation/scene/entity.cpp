//
//  entity.cpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#include "entity.hpp"

namespace ikan {
  
  Entity::Entity() {
    IK_CORE_TRACE("Entity", "Creating Default Entity with No Handle ...");
  }
  
  Entity::Entity(entt::entity handle, EnttScene* scene)
  : entity_handle_(handle), scene_(scene) {
    IK_CORE_TRACE("Entity", "Creating Entity with Handle: {0} ...", (uint32_t)entity_handle_);
  }
  
  Entity::Entity(Entity&& other) : entity_handle_(other.entity_handle_),
  scene_(other.scene_) {
    IK_CORE_TRACE("Entity", "Moving Entity with Handle: {0} ...", (uint32_t)entity_handle_);
  }
  
  Entity::Entity(const Entity& other) : entity_handle_(other.entity_handle_),
  scene_(other.scene_) {
    IK_CORE_TRACE("Entity", "Copying Entity with Handle: {0} ...", (uint32_t)entity_handle_);
  }
  
  Entity& Entity::operator=(const Entity& other) {
    IK_CORE_TRACE("Entity", "Copying Entity with Handle withg = operator {0} ...", (uint32_t)entity_handle_);
    entity_handle_ = other.entity_handle_;
    scene_ = other.scene_;
    return *this;
  }
  
  Entity& Entity::operator=(Entity&& other) {
    IK_CORE_TRACE("Entity", "Moving Entity with Handle withg = operator {0} ...", (uint32_t)entity_handle_);
    entity_handle_ = other.entity_handle_;
    scene_ = other.scene_;
    return *this;
  }
  
  Entity::~Entity() {
    IK_CORE_WARN("Entity", "Destroying Entity with Handle: {0} !!!", (uint32_t)entity_handle_);
  }

  Entity::operator entt::entity() const { return entity_handle_; }

}
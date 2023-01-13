//
//  entity.cpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#include "entity.hpp"
#include "ecs/core_components.hpp"

namespace ecs {
  
  Entity::Entity() {
    IK_CORE_TRACE(LogModule::Entity, "Creating Default Entity with No Handle ...");
  }
  
  Entity::Entity(entt::entity handle, EnttScene* scene)
  : entity_handle_(handle), scene_(scene) {
    IK_CORE_TRACE(LogModule::Entity, "Creating Entity with Handle: {0} ...", (uint32_t)entity_handle_);
  }
  
  Entity::Entity(Entity&& other) : entity_handle_(other.entity_handle_),
  scene_(other.scene_) {
    IK_CORE_TRACE(LogModule::Entity, "Moving Entity with Handle: {0} ...", (uint32_t)entity_handle_);
  }
  
  Entity::Entity(const Entity& other) : entity_handle_(other.entity_handle_),
  scene_(other.scene_) {
    IK_CORE_TRACE(LogModule::Entity, "Copying Entity with Handle: {0} ...", (uint32_t)entity_handle_);
  }
  
  Entity& Entity::operator=(const Entity& other) {
    entity_handle_ = other.entity_handle_;
    scene_ = other.scene_;
    IK_CORE_TRACE(LogModule::Entity, "Copying Entity using = operator with Handle {0} ...", (uint32_t)entity_handle_);
    return *this;
  }
  
  Entity& Entity::operator=(Entity&& other) {
    entity_handle_ = other.entity_handle_;
    scene_ = other.scene_;
    IK_CORE_TRACE(LogModule::Entity, "Moving Entity using = operator with Handle {0} ...", (uint32_t)entity_handle_);
    return *this;
  }
  
  Entity::~Entity() {
    IK_CORE_WARN(LogModule::Entity, "Destroying Entity with Handle: {0} !!!", (uint32_t)entity_handle_);
  }

  Entity::operator entt::entity() const { return entity_handle_; }
  Entity::operator uint32_t() const { return (uint32_t)entity_handle_; }
  Entity::operator bool() const { return entity_handle_ != entt::null; }
  bool Entity::operator ==(const Entity& other) const { return entity_handle_ == other.entity_handle_ and scene_ == other.scene_; }
  bool Entity::operator !=(const Entity& other) const { return !(*this == other); }
  bool Entity::operator ==(const entt::entity& other) const { return entity_handle_ == other; }
  bool Entity::operator !=(const entt::entity& other) const { return !(*this == other); }

  UUID Entity::GetUUID() const { return GetComponent<IDComponent>().id; }
  const std::string& Entity::GetName() const { return GetComponent<TagComponent>().tag; }

}

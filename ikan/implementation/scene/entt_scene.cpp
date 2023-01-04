//
//  scene.cpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#include "entt_scene.hpp"
#include "scene/entity.hpp"
#include "scene/core_components.hpp"

namespace ikan {
  
  EnttScene::EnttScene() {
    IK_CORE_INFO("EnTT Scene", "Creating Scene ...");
  }
  
  EnttScene::~EnttScene() {
    IK_CORE_WARN("EnTT Scene", "Destroying Scene!!!");
  }
  
  Entity EnttScene::CreateEntity(const std::string& name, UUID uuid) {
    Entity entity {registry_.create(), this};

    // Add the Mendatory components
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TagComponent>(name);

    IK_CORE_TRACE("EnTT Scene", "Stored Entity in Scene");
    IK_CORE_TRACE("EnTT Scene", "  Name | {0}", entity.GetComponent<TagComponent>().tag.c_str());
    IK_CORE_TRACE("EnTT Scene", "  ID   | {0}", entity.GetComponent<IDComponent>().id);
    return entity;
  }
  
  void EnttScene::DestroyEntity(Entity entity) {
    IK_CORE_WARN("EnTT Scene", "Removed Entity from Scene");
    IK_CORE_WARN("EnTT Scene", "  Name | {0}", entity.GetComponent<TagComponent>().tag.c_str());
    IK_CORE_WARN("EnTT Scene", "  ID   | {0}", entity.GetComponent<IDComponent>().id);
    registry_.destroy(entity);
  }

}

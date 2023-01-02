//
//  scene.cpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#include "entt_scene.hpp"
#include "scene/entity.hpp"

namespace ikan {
  
  EnttScene::EnttScene() {
    IK_INFO("Creating Scene ...");
  }
  
  EnttScene::~EnttScene() {
    IK_WARN("Destroying Scene!!!");
  }
  
  Entity EnttScene::CreateEntity() {
    Entity entity {registry_.create(), this};
    IK_TRACE("Stored Entity in Scene");
    return entity;
  }
  
  void EnttScene::DestroyEntity(Entity entity) {
    IK_WARN("Removed Entity from Scene");
    registry_.destroy(entity);
  }

}

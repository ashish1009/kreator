//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#include "player.hpp"
#include "camera_controller.h"

namespace mario {
  
  Player::Player(EnttScene* scene)
  : scene_(scene) {
    IK_INFO("Mario", "Creating Mario Player");
    
    player_entity_ = scene_->CreateEntity("Player");
    player_entity_.AddComponent<QuadComponent>();
    
    AABB aabb;
    aabb.min = { -0.5f, -0.5f, -0.5f };
    aabb.max = { 0.5f, 0.5f, 0.5f };
    player_entity_.AddComponent<RigidBodyComponent>(aabb);
    
    player_entity_.AddComponent<NativeScriptComponent>(nullptr).Bind<ikan::FreeFallController>();
  }
  
  Player::~Player() {
    IK_WARN("Mario", "Destroying Mario Player");
  }
  
}

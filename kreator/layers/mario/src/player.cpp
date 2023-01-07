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
    player_entity_.AddComponent<NativeScriptComponent>(nullptr).Bind<ikan::FreeFallController>();
  }
  
  Player::~Player() {
    IK_WARN("Mario", "Destroying Mario Player");
  }
  
}

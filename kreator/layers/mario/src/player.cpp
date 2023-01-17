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
  }
  
  Player::~Player() {
    IK_WARN("Mario", "Destroying Mario Player");
  }
  
#if MARIO_DEBUG
  void Player::RenderGui() {
    ImGui::Begin("Player Pos");
    ImGui::PushID("Player Pos");
    
    player_entity_.GetComponent<TransformComponent>().RenderGui();
    
    ImGui::PopID();
    ImGui::End();
  }
  
  void Player::Reset() {
    player_entity_.GetComponent<TransformComponent>().UpdateTranslation({0, 0, 0});
  }
#endif
}

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
    
    auto& native_script_comp = player_entity_.AddComponent<NativeScriptComponent>([](NativeScriptComponent* sc,
                                                                                    const std::string& script_name) {
      if (script_name == "mario::PlayerController") {
        sc->Bind<mario::PlayerController>();
        return true;
      }
      return false;
    });
    native_script_comp.Bind<ikan::FreeFallController>();
    native_script_comp.Bind<mario::PlayerController>();
    
    
  }
  
  Player::~Player() {
    IK_WARN("Mario", "Destroying Mario Player");
  }
  
  void PlayerController::Update(Timestep ts) {
    // Dummy copy of entity y Position
    auto& translation = GetComponent<TransformComponent>().translation;
    if (Input::IsKeyPressed(KeyCode::Left))
      translation.x -= speed_ * ts;
    if (Input::IsKeyPressed(KeyCode::Right))
      translation.x += speed_ * ts;
  }
  
}

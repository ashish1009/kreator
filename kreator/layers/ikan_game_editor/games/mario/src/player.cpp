//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#include "player.hpp"

namespace mario {

  PlayerController* PlayerController::instance_ = nullptr;
  
  StateMachine::StateMachine(Entity* entity) {
    player_entity_ = entity;
  }
  
  PlayerController::PlayerController() {
    instance_ = this;
  }
  
  PlayerController::~PlayerController() {
    delete state_machine_;
  }
  
  PlayerController* PlayerController::Get() {
    return instance_;
  }

  void PlayerController::Create(Entity entity)  {
    entity_ = entity;
    state_machine_ = new StateMachine(&entity_);

    if (state_machine_->player_state_ == PlayerState::Small) {
      player_width_ = 1.0f;
      player_height_ = 1.0f;
    }
    else if (state_machine_->player_state_ == PlayerState::Big) {
      player_width_ = 1.0f;
      player_height_ = 2.0f;
    }
  }
  
  void PlayerController::Update(Timestep ts) {
    CheckOnGround();
  }
  
  void PlayerController::CheckOnGround() {
    float inner_player_width = player_width_ * 0.6f;
    float y_val = -(player_height_ / 2);
    y_val -= (state_machine_->player_state_ == PlayerState::Small) ? 0.02f : 0.02f;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
  }

  void PlayerController::RenderGui() {
    ImGui::Text(" On Ground     | %s", on_ground_ ? "True" : "False");
  }
  
}
 

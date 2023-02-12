//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#include "player.hpp"

namespace mario {
    
  void StateMachine::Update() {
    switch (state_) {
      case PlayerState::Idle:
        break;
      case PlayerState::Run:
        break;
      case PlayerState::SwitchSide:
        break;
      case PlayerState::Jump:
        break;
      case PlayerState::Die:
        break;
      case PlayerState::BigIdle:
        break;
      case PlayerState::BigRun:
        break;
      case PlayerState::BigJump:
        break;
      case PlayerState::BigSwitchSide:
        break;
      case PlayerState::FireIdle:
        break;
      case PlayerState::FireRun:
        break;
      case PlayerState::FireJump:
        break;
      case PlayerState::FireSwitchSide:
        break;
    }
  }
    
  void PlayerController::Create(Entity entity)  {
    entity_ = entity;

    rigid_body_comp_ = &(GetComponent<RigidBodyComponent>());
    rigid_body_comp_->SetGravityScale(0.0f);
    
    state_machine_ = new StateMachine(&entity_);
  }
  
  void PlayerController::Update(Timestep ts) { // Run Left
    auto& tc = entity_.GetComponent<TransformComponent>();
    state_machine_->Update();
    
    if (Input::IsKeyPressed(KeyCode::Left)) {
      tc.UpdateScale_X(-player_width_);
      acceleration_.x = -warlk_speed_;
      
      if (velocity_.x > 0) {
        velocity_.x -= slow_down_force_;
        state_machine_->ChangeState(PlayerState::SwitchSide);
        // State Machine change direction
      }
      else {
        // State Machine Running
      }
    }
    else if (Input::IsKeyPressed(KeyCode::Right)) { // Run Right
      tc.UpdateScale_X(player_width_);
      acceleration_.x = warlk_speed_;
      
      if (velocity_.x < 0) {
        velocity_.x += slow_down_force_;
        state_machine_->ChangeState(PlayerState::SwitchSide);
      }
      else {
        // State Machine Running
      }
    }
    else { // Friction Stop
      acceleration_.x = 0;
      if (velocity_.x > 0) {
        velocity_.x = std::max(0.0f, velocity_.x - slow_down_force_);
      }
      else if (velocity_.x < 0) {
        velocity_.x = std::min(0.0f, velocity_.x + slow_down_force_);
      }
      
      if (velocity_.x == 0) {
        // State change stop running / Idle
      }
    }
    
    acceleration_.y = -9.8;
    
    velocity_.x += acceleration_.x * ts;
    velocity_.y += acceleration_.y * ts;
    
    velocity_.x = std::max(std::min(velocity_.x, terminal_velocity_.x), -terminal_velocity_.x);
    velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
   
    rigid_body_comp_->SetVelocity(velocity_);
    rigid_body_comp_->SetAngularVelocity(0.0f);
  }
  
  void PlayerController::RenderGui() {
    ImGui::Text(" %f %f", acceleration_.x, acceleration_.y);
  }
  
}

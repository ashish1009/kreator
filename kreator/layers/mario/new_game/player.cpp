//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#include "player.hpp"
#include "sprite_manager.hpp"

namespace mario {
    
  void StateMachine::Update() {
    switch (state_) {
      case PlayerState::Idle:
        break;
      case PlayerState::Run: {
        static const auto& sprites = SpriteManager::GetPlayerStateSprite(PlayerState::Run);
        static const int32_t speed = 10;
        static int32_t anim_idx = 0;

        auto& qc = player_entity_->GetComponent<QuadComponent>();
        
        if (anim_idx >= speed * sprites.size() or anim_idx < 1)
          anim_idx = 0;
          
        qc.texture_comp.sprite = sprites[anim_idx / speed];

        anim_idx++;
        break;
      }
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
  
  void StateMachine::ChangeState(PlayerState state) {
    state_ = state;

    if (state_ != PlayerState::Run) {
      auto& qc = player_entity_->GetComponent<QuadComponent>();
      qc.texture_comp.sprite = SpriteManager::GetPlayerStateSprite(state_)[0];
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
      }
      else {
        state_machine_->ChangeState(PlayerState::Run);
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
        state_machine_->ChangeState(PlayerState::Run);
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
        state_machine_->ChangeState(PlayerState::Idle);
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

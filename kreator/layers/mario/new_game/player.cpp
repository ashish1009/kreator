//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#include "player.hpp"

namespace mario {
  
  StateMachine::StateMachine(Entity* entity) : entity_(entity) {
  }
  
  void StateMachine::ExtractData() {
  }
  
  void StateMachine::ChangeState(State state) {
    if (state == State::Run) {
      auto& ac = entity_->GetComponent<AnimationComponent>();
      ac.animation = true;
    }
    else if (state == State::SwitchDirection) {
      static std::shared_ptr<SubTexture> switch_dir = SubTexture::CreateFromCoords(sprite_image_, {}, sprite_size_, cell_size_);
    }
  }
  
  StateMachine::State StateMachine::GetState() const {
    return state_;
  }
  
  PlayerController::PlayerController(RigidBodyComponent* rb)
  : rigid_body_comp_(rb), state_machine_(&entity_) {
    rigid_body_comp_->SetGravityScale(0.0f);
  }
  
  void PlayerController::Update(Timestep ts) {
    auto& tc = GetComponent<TransformComponent>();
    
    if (Input::IsKeyPressed(KeyCode::Left)) {
      state_machine_.ChangeState(StateMachine::State::Run);
      
      tc.UpdateScale_X(-player_width_);
      acceleration_.x = -warlk_speed_;
      
      if (velocity_.x > 0) {
        velocity_.x -= slow_down_force_;
        // State Machine change direction
      }
      else {
        // State Machine Running
      }
    }
    if (Input::IsKeyPressed(KeyCode::Right)) {
      state_machine_.ChangeState(StateMachine::State::Run);
      
      tc.UpdateScale_X(player_width_);
      acceleration_.x = warlk_speed_;
      
      if (velocity_.x < 0) {
        velocity_.x += slow_down_force_;
        // State Machine change direction
      }
      else {
        // State Machine Running
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
  
}

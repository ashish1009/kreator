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
    
    SetAction(PlayerAction::Idle);
  }
  
  std::string StateMachine::StateString() const {
    switch (player_state_) {
      case PlayerState::Invalid:      IK_ASSERT(false);
      case PlayerState::Small:        return "Small";
      case PlayerState::Big:          return "Big";
      case PlayerState::Fire:         return "Fire";
      case PlayerState::Invicible:    return "Invicible";
    }
  }
  
  std::string StateMachine::ActionString() const {
    switch (player_action_) {
      case PlayerAction::Invalid:     IK_ASSERT(false);
      case PlayerAction::Idle:        return "Idle";
      case PlayerAction::Run:         return "Running";
      case PlayerAction::SwitchSide:  return "Switch Side";
      case PlayerAction::Jump:        return "Jumping";
      case PlayerAction::Die:         return "Dying";
      case PlayerAction::PowerUp:     return "PowerUp";
    }
  }
  
  void StateMachine::Update(Timestep ts) {
    switch (player_action_) {
      case PlayerAction::Invalid: {
        IK_ASSERT(false);
      }
      case PlayerAction::Idle: {
        break;
      }
      case PlayerAction::Run: {
        break;
      }
      case PlayerAction::SwitchSide: {
        break;
      }
      case PlayerAction::Jump: {
        break;
      }
      case PlayerAction::Die: {
        break;
      }
      case PlayerAction::PowerUp: {
        break;
      }
    }
  }
  
  void StateMachine::SetAction(PlayerAction new_action) {
    player_prev_action_ = player_action_;
    player_action_ = new_action;
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

    // Set the state of player
    ChangeState(PlayerState::Small);
        
    // Disbale Gravity on player
    GetComponent<RigidBodyComponent>().SetGravityScale(0.0f);
  }
  
  void PlayerController::Update(Timestep ts) {
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    const auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
    
    if (reset_fixture_) {
      EnttScene::ResetPillBoxColliderFixture(entity_.GetComponent<TransformComponent>(), &rb, pbc);
      reset_fixture_ = false;
    }

    CheckOnGround();
  }
  
  void PlayerController::CheckOnGround() {
    float inner_player_width = player_width_ * 0.6f;
    float y_val = -(player_height_ / 2);
    y_val -= 0.02f;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
  }
  
  void PlayerController::ChangeState(PlayerState new_state) {
    state_machine_->SetState(new_state);
    
    if (state_machine_->State() == PlayerState::Small) {
      player_width_ = 1.0f;
      player_height_ = 1.0f;
    }
    else if (state_machine_->State() == PlayerState::Big) {
      player_width_ = 1.0f;
      player_height_ = 2.0f;
    }
    
    auto& tc = entity_.GetComponent<TransformComponent>();
    tc.UpdateScale_Y(player_height_);

    // Add Impulse to push player out of ground while changing size
    entity_.GetComponent<RigidBodyComponent>().AddVelocity({velocity_.x, 1000.0});
    auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
    pbc.SetSize({0.5f, player_height_ / 2.0f});
    
    reset_fixture_ = true;
  }

  void PlayerController::RenderGui() {
    if (state_machine_) {
      ImGui::Text(" State             | %s", state_machine_->StateString().c_str());
      ImGui::Text(" Action            | %s", state_machine_->ActionString().c_str());
    }
    
    ImGui::Text(" On Ground         | %s", on_ground_ ? "True" : "False");

    ImGui::Text(" Width             | %f", player_width_);
    ImGui::Text(" Height            | %f", player_height_);

    ImGui::Text(" Accelaration      | %f - %f", acceleration_.x, acceleration_.y);
    ImGui::Text(" Velocity          | %f - %f", velocity_.x, velocity_.y);
    ImGui::Text(" Terminal Velocity | %f - %f", terminal_velocity_.x, terminal_velocity_.y);

    ImGui::Text(" Walk Speed        | %f", walk_speed_);
    ImGui::Text(" Slow Down Force   | %f", slow_down_force_);
    ImGui::Text(" Free Fall Factor  | %f", free_fall_factor);
  }
  
}
 

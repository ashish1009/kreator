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
    ImGui::Text(" On Ground     | %s", on_ground_ ? "True" : "False");
  }
  
}
 

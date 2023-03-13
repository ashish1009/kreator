//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#include "player.hpp"
#include "sprite_manager.hpp"

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
    auto& qc = player_entity_->GetComponent<QuadComponent>();

    switch (player_action_) {
      case PlayerAction::Invalid: {
        IK_ASSERT(false);
      }
      case PlayerAction::Idle:
      case PlayerAction::SwitchSide:
      case PlayerAction::Jump:
      case PlayerAction::Die:
        break;
      case PlayerAction::PowerUp: {
        // Animation of player while powerup fire
        if (player_state_ == PlayerState::Fire) {
          static const int32_t invince_speed = 10;
          static int32_t invinc_anim_idx = 0;
          static std::array<float, 5> incince_sprite_y_idx = { 31, 28, 25, 22, 19 };
          static auto sprite_image = SpriteManager::GetSpriteImage(SpriteType::Player);
          static auto prev_sprite_x_coord = qc.texture_comp.sprite->GetCoords().x;
          
          if (invinc_anim_idx >= invince_speed * incince_sprite_y_idx.size() or invinc_anim_idx < 1)
            invinc_anim_idx = 0;
          
          qc.texture_comp.sprite = SubTexture::CreateFromCoords(sprite_image,
                                                                {prev_sprite_x_coord, incince_sprite_y_idx[invinc_anim_idx / invince_speed]},
                                                                {1.0f, 2.0f});
          invinc_anim_idx++;
        }
        break;
      }
        
      case PlayerAction::Run: {
        // Animation of player while running
        static const int32_t run_speed = 10;
        static int32_t run_anim_idx = 0;

        if (run_anim_idx >= run_speed * running_player_sprites_->size() or run_anim_idx < 1)
          run_anim_idx = 0;
          
        qc.texture_comp.sprite = running_player_sprites_->at(run_anim_idx / run_speed);
        run_anim_idx++;

        break;
      }
    }
  }
  
  void StateMachine::SetState(PlayerState new_state) {
    player_prev_state_ = player_state_;
    player_state_ = new_state;
    
    // Store the running player animation sprite based on the state of player
    running_player_sprites_ = &SpriteManager::GetPlayerStateSprite(player_state_, PlayerAction::Run);
  }
  
  void StateMachine::SetAction(PlayerAction new_action) {
    player_prev_action_ = player_action_;
    player_action_ = new_action;
    
    auto& qc = player_entity_->GetComponent<QuadComponent>();
    // We want an animation while running and powerup so not changing sprite while changing the action
    // Sprite animation taken  care at state_machine update function
    if (player_action_ != PlayerAction::Run and player_action_ != PlayerAction::PowerUp) {
      qc.texture_comp.sprite = SpriteManager::GetPlayerStateSprite(player_state_, player_action_)[0];
    }
    else {
      if (player_action_ == PlayerAction::PowerUp) {
        qc.texture_comp.sprite = SpriteManager::GetPlayerStateSprite(player_state_, player_prev_action_)[0];
      }
    }
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
    SetState(PlayerState::Small);
        
    // Disbale Gravity on player
    GetComponent<RigidBodyComponent>().SetGravityScale(0.0f);
  }
  
  void PlayerController::Update(Timestep ts) {
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    const auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
    
    // Following should be in same order

    // 1. Reset the fixture if size changed
    if (reset_fixture_) {
      EnttScene::ResetPillBoxColliderFixture(entity_.GetComponent<TransformComponent>(), &rb, pbc);
      reset_fixture_ = false;
    }
    
    // 2. Update the state machine
    state_machine_->Update(ts);

    // 3. Check player on ground or not
    CheckOnGround();
    
    // 4. Check is player hits the powerup item
    if (power_up_)
      PowerUp(ts);
    
    // 5. Freeze until player power up completes
    if (state_machine_->Action() == PlayerAction::PowerUp) {
      freez_time_-= ts;
      if (freez_time_ >= 0) {
        if (!on_ground_) {
          rb.SetVelocity({0, 0});
        }
        return;
      }
      else {
        state_machine_->SetAction(state_machine_->PrevAction());
        freez_time_ = 0.5f;
        
        if (state_machine_->State() == PlayerState::Big) {
          // As our player powered up so reset the pill box size
          reset_fixture_ = true;
        }
      }
    }

    // 6. Poll the buttons for Running the player
    Run(ts);
    
    // 7. Poll the button for Running the jumping
    JumpAndBounce(ts);
    
    // 8. Add Velocity to player body
    velocity_.x += acceleration_.x * ts * 2.0f;
    velocity_.y += acceleration_.y * ts * 2.0f;
    
    velocity_.x = std::max(std::min(velocity_.x, terminal_velocity_.x), -terminal_velocity_.x);
    velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
    
    rb.SetVelocity(velocity_);
    rb.SetAngularVelocity(0.0f);
  }
  
  void PlayerController::BeginCollision(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    if (is_dying_)
      return;

    // Immediatly kill all forward velocity as player hits the obstacle of block
    if (std::abs(contact_normal.x) > 0.8f) {
      velocity_.x = 0.0f;
    }
    
    else if (contact_normal.y > 0.8f) {
      velocity_.y = 0;
      acceleration_.y = 0;
      jump_time_ = 0;
    }
  }
  
  void PlayerController::PowerUp(Timestep ts) {
    if (IsSmall()) {
      SetState(PlayerState::Big);
      
      jumb_boost_ *= big_jump_boost_factor_;
      walk_speed_ *= big_jump_boost_factor_;
      
      // When player will hurt it will blink for this time
      blink_time_ = hurt_invincibility_time_;
    }
    else {
      SetState(PlayerState::Fire);
    }
    
    state_machine_->SetAction(PlayerAction::PowerUp);
    power_up_ = false;
  }
  
  void PlayerController::Run(Timestep ts) {
    auto& tc = entity_.GetComponent<TransformComponent>();

    if (Input::IsKeyPressed(KeyCode::Left)) {
      tc.UpdateScale_X(-player_width_);
      acceleration_.x = -walk_speed_;
      
      if (velocity_.x > 0) {
        velocity_.x -= slow_down_force_;
        state_machine_->SetAction(PlayerAction::SwitchSide);
      }
      else {
        state_machine_->SetAction(PlayerAction::Run);
      }
    }
    else if (Input::IsKeyPressed(KeyCode::Right)) { // Run Right
      tc.UpdateScale_X(player_width_);
      acceleration_.x = walk_speed_;
      
      if (velocity_.x < 0) {
        velocity_.x += slow_down_force_;
        state_machine_->SetAction(PlayerAction::SwitchSide);
      }
      else {
        state_machine_->SetAction(PlayerAction::Run);
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
        state_machine_->SetAction(PlayerAction::Idle);
      }
    }
  }
  
  void PlayerController::JumpAndBounce(Timestep ts) {
    if (Input::IsKeyPressed(KeyCode::Space) and (jump_time_ > 0 or on_ground_ or ground_debounce_ > 0)) {
      if ((on_ground_ or ground_debounce_ > 0) and jump_time_ == 0) {
        jump_time_ = 55;
        velocity_.y = jump_impuls_;
      }
      else if (jump_time_ > 0) {
        jump_time_--;
        velocity_.y = (jump_time_ / 2.2f) * jumb_boost_;
      }
      else {
        velocity_.y = 0.0f;
      }
      ground_debounce_ = 0.0f;
      
      // Player State is Jumping if player is on Air
      state_machine_->SetAction(PlayerAction::Jump);
    }
    else if (enemy_bounce_ > 0) {
      enemy_bounce_--;
      velocity_.y = ((enemy_bounce_ / 2.2f) * jumb_boost_);
    }
    else if (!on_ground_) {
      if (jump_time_ > 0) {
        velocity_.y *= 0.35f;
        jump_time_ = 0;
      }
      ground_debounce_ -= ts;
      
      // Free fall with scene gravity
      acceleration_.y = entity_.GetScene()->GetGravity().y * free_fall_factor;
      
      // Player State is Jumping if player is on Air
      state_machine_->SetAction(PlayerAction::Jump);
    }
    else {
      velocity_.y = 0;
      acceleration_.y = 0;
      ground_debounce_ = ground_debounce_time_;
    }
  }
  
  void PlayerController::CheckOnGround() {
    float inner_player_width = player_width_ * 0.6f;
    float y_val = -(player_height_ / 2);
    y_val -= 0.02f;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
  }
  
  void PlayerController::SetState(PlayerState new_state) {
    state_machine_->SetState(new_state);
    
    auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
    if (state_machine_->State() == PlayerState::Small) {
      player_width_ = 1.0f;
      player_height_ = 1.0f;
      pbc.SetSize({0.4f, player_height_ / 2.0f});
    }
    else if (state_machine_->State() == PlayerState::Big) {
      player_width_ = 1.0f;
      player_height_ = 2.0f;

      // Add Impulse to push player out of ground while changing size
      entity_.GetComponent<RigidBodyComponent>().AddVelocity({velocity_.x, 1000.0});
      pbc.SetSize({0.5f, player_height_ / 2.0f});
    }
    else if (state_machine_->State() == PlayerState::Fire) {
      // Do Nothing for Fire for now
    }
    entity_.GetComponent<TransformComponent>().UpdateScale_Y(player_height_);

    reset_fixture_ = true;
  }
  
  void PlayerController::Copy(void *script) {
    if (!script)
      return;
    
    PlayerController* player_script = reinterpret_cast<PlayerController*>(script);
    IK_ASSERT(player_script);

    reset_fixture_ = player_script->reset_fixture_;
    on_ground_ = player_script->on_ground_;
    is_dead_ = player_script->is_dead_;
    is_dying_ = player_script->is_dying_;
    power_up_ = player_script->power_up_;
    
    hurt_invincibility_time_ = player_script->hurt_invincibility_time_;
    hurt_invincibility_time_left_ = player_script->hurt_invincibility_time_left_;
    blink_time_ = player_script->blink_time_;
    
    player_width_ = player_script->player_width_;
    
    free_fall_factor = player_script->free_fall_factor;
    
    walk_speed_ = player_script->walk_speed_;
    slow_down_force_ = player_script->slow_down_force_;
    
    ground_debounce_ = player_script->ground_debounce_;
    ground_debounce_time_ = player_script->ground_debounce_time_;
    
    jumb_boost_ = player_script->jumb_boost_;
    big_jump_boost_ = player_script->big_jump_boost_;
    jump_impuls_ = player_script->jump_impuls_;
    big_jump_boost_factor_ = player_script->big_jump_boost_factor_;
    
    freez_time_ = player_script->freez_time_;
    
    enemy_bounce_ = player_script->enemy_bounce_;
    jump_time_ = player_script->jump_time_;
    
    acceleration_ = player_script->acceleration_;
    velocity_ = player_script->velocity_;
    terminal_velocity_ = player_script->terminal_velocity_;
    
    score_ = player_script->score_;
    coins_ = player_script->coins_;
  }

  void PlayerController::RenderGui() {
    if (state_machine_) {
      ImGui::Text(" State             | %s", state_machine_->StateString().c_str());
      ImGui::Text(" Action            | %s", state_machine_->ActionString().c_str());
    }
    ImGui::Text(" On Ground         | %s", on_ground_ ? "True" : "False");
  }
  
}
 

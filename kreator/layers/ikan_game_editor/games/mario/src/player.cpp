//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "player.hpp"
#include "sprite_manager.hpp"

namespace mario {
  
  void StateMachine::Update(Timestep ts) {
    switch (player_action_) {
      case PlayerAction::Idle:
        break;
      case PlayerAction::Run: {
        static const auto& small_run_sprites = SpriteManager::GetPlayerStateSprite(PlayerState::Small, player_action_);
        static const auto& big_run_sprites = SpriteManager::GetPlayerStateSprite(PlayerState::Big, player_action_);
        static const auto& fire_run_sprites = SpriteManager::GetPlayerStateSprite(PlayerState::Fire, player_action_);
        
        static const int32_t run_speed = 10;
        static int32_t run_anim_idx = 0;
        
        auto& qc = player_entity_->GetComponent<QuadComponent>();
        
        if (*player_state_ == PlayerState::Small) {
          if (run_anim_idx >= run_speed * small_run_sprites.size() or run_anim_idx < 1)
            run_anim_idx = 0;
          
          qc.texture_comp.sprite = small_run_sprites[run_anim_idx / run_speed];
        }
        else if (*player_state_ == PlayerState::Big){
          if (run_anim_idx >= run_speed * big_run_sprites.size() or run_anim_idx < 1)
            run_anim_idx = 0;
          
          qc.texture_comp.sprite = big_run_sprites[run_anim_idx / run_speed];
        }
        else {
          if (run_anim_idx >= run_speed * fire_run_sprites.size() or run_anim_idx < 1)
            run_anim_idx = 0;
          
          qc.texture_comp.sprite = fire_run_sprites[run_anim_idx / run_speed];
        }
        
        run_anim_idx++;
        break;
      }
      case PlayerAction::SwitchSide:
        break;
      case PlayerAction::Jump:
        break;
      case PlayerAction::Die:
        break;
      case PlayerAction::PowerUp: {
        // Animation for Flower collision
        if (*player_state_ == PlayerState::Fire) {
          auto& qc = player_entity_->GetComponent<QuadComponent>();
          static const int32_t speed = 10;
          static int32_t anim_idx = 0;
          static float y_idx[5] = { 31, 28, 25, 22, 19 };
          static auto sprite_image = SpriteManager::GetSpriteImage(SpriteType::Player);
          
          if (anim_idx >= speed * 5 or anim_idx < 1)
            anim_idx = 0;
          
          qc.texture_comp.sprite = SubTexture::CreateFromCoords(sprite_image, {6.0f, y_idx[anim_idx / speed]}, {1.0f, 2.0f});
          anim_idx++;
        }
        break;
      }
    }
  }
  
  void StateMachine::ChangeAction(PlayerAction action) {
    prev_action_ = player_action_;
    player_action_ = action;
    auto& qc = player_entity_->GetComponent<QuadComponent>();
    if (player_action_ != PlayerAction::Run and player_action_ != PlayerAction::PowerUp) {
      qc.texture_comp.sprite = SpriteManager::GetPlayerStateSprite(*player_state_, player_action_)[0];
    }
    else {
      if (player_action_ == PlayerAction::PowerUp) {
        qc.texture_comp.sprite = SpriteManager::GetPlayerStateSprite(*player_state_, prev_action_)[0];
      }
    }
  }
  
  PlayerController* PlayerController::instance_ = nullptr;
  
  PlayerController::PlayerController() {
    instance_ = this;
  }
  
  PlayerController::~PlayerController() {
    delete state_machine_;
  }

  void PlayerController::Create(Entity entity)  {
    entity_ = entity;
    state_machine_ = new StateMachine(&entity_, &player_state_);
    
    auto& rb = GetComponent<RigidBodyComponent>();
    rb.SetGravityScale(0.0f);
  }

  void PlayerController::Update(Timestep ts) {
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    const auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
    
    if (reset_fixture_) {
      EnttScene::ResetPillBoxColliderFixture(entity_.GetComponent<TransformComponent>(), &rb, pbc);
      reset_fixture_ = false;
    }

//    if (is_dead_) {
//      EnttScene::ResetPillBoxColliderFixture(entity_.GetComponent<TransformComponent>(), &rb, pbc);
//      velocity_.x = 0.0f;
//
//      auto& tc = entity_.GetComponent<TransformComponent>();
//      if (tc.Translation().y < dead_max_height_ and dead_going_up_) {
//        acceleration_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y;
//
//        velocity_.y += acceleration_.y * ts * 2.0f;
//        velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
//
//        rb.SetVelocity({velocity_.x, -velocity_.y});
//        rb.SetAngularVelocity(0.0f);
//      }
//      else if (tc.Translation().y >= dead_max_height_ and dead_going_up_) {
//        dead_going_up_ = false;
//      }
//      else if (!dead_going_up_) {
//        acceleration_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y;
//
//        velocity_.y += acceleration_.y * ts * 2.0f;
//        velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
//
//        rb.SetVelocity(velocity_);
//        rb.SetAngularVelocity(0.0f);
//      }
//      // Stop at some point . End game reset level
//      return;
//    }
    
    if (hurt_invincibility_time_left_ > 0) {
      hurt_invincibility_time_left_ -= ts;
      blink_time_ -= ts;
      
      auto& qc = entity_.GetComponent<QuadComponent>();
      if (blink_time_ >= 0) {
        qc.color.a = a_[(blink_a_idx_++ / blink_speed_) % 2];
      }
      else {
        qc.color.a = 1.0f;
        
        auto& rb = entity_.GetComponent<RigidBodyComponent>();
        rb.is_sensor = false;
        reset_fixture_ = true;
      }
    }
    else {
    }
    
    CheckOnGround();
    state_machine_->Update(ts);
    
    if (power_up_)
      Powerup();
    
    // Freeze until player power up complets
    if (state_machine_->GetAction() == PlayerAction::PowerUp) {
      freez_time_-= ts;
      if (freez_time_ >= 0) {
        if (!on_ground_) {
          rb.SetVelocity({0, 0});
        }
        return;
      }
      else {
        state_machine_->ChangeAction(state_machine_->GetPrevAction());
        freez_time_ = 0.5f;
        
        if (player_state_ == PlayerState::Big) {
          // As our player powered up so reset the pill box size
          reset_fixture_ = true;
        }
      }
    }
    
    auto& tc = entity_.GetComponent<TransformComponent>();
    
    if (Input::IsKeyPressed(KeyCode::Left)) {
      tc.UpdateScale_X(-player_width_);
      acceleration_.x = -walk_speed_;
      
      if (velocity_.x > 0) {
        velocity_.x -= slow_down_force_;
        state_machine_->ChangeAction(PlayerAction::SwitchSide);
      }
      else {
        state_machine_->ChangeAction(PlayerAction::Run);
      }
    }
    else if (Input::IsKeyPressed(KeyCode::Right)) { // Run Right
      tc.UpdateScale_X(player_width_);
      acceleration_.x = walk_speed_;
      
      if (velocity_.x < 0) {
        velocity_.x += slow_down_force_;
        state_machine_->ChangeAction(PlayerAction::SwitchSide);
      }
      else {
        state_machine_->ChangeAction(PlayerAction::Run);
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
        state_machine_->ChangeAction(PlayerAction::Idle);
      }
    }
    
    if (Input::IsKeyPressed(KeyCode::Space) and (jump_time_ > 0 or on_ground_ or ground_debounce_ > 0)) {
      if ((on_ground_ or ground_debounce_ > 0) and jump_time_ == 0) { // Just Press Jump Key
                                                                      // Play Sound
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
      acceleration_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * free_fall_factor;
    }
    else { // Landed on ground
      velocity_.y = 0;
      acceleration_.y = 0;
      ground_debounce_ = ground_debounce_time_;
    }
    
    velocity_.x += acceleration_.x * ts * 2.0f;
    velocity_.y += acceleration_.y * ts * 2.0f;
    
    velocity_.x = std::max(std::min(velocity_.x, terminal_velocity_.x), -terminal_velocity_.x);
    velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
    
    rb.SetVelocity(velocity_);
    rb.SetAngularVelocity(0.0f);
    
    if (!on_ground_) {
      state_machine_->ChangeAction(PlayerAction::Jump);
    }
    else {
//      state_machine_->ChangeState(PlayerState::Idle);
    }
  }
  
  void PlayerController::CheckOnGround() {
    float inner_player_width = player_width_ * 0.6f;
    float y_val = -(player_height_ / 2);
    y_val -= (player_state_ == PlayerState::Small) ? 0.02f : 0.02f;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
  }

  void PlayerController::BeginCollision(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    if (is_dead_ or !collided_entity or !collided_entity->GetScene())
      return;
    
//    if (collided_entity->HasComponent<RigidBodyComponent>()) {
      if (std::abs(contact_normal.x) > 0.8f) {
        velocity_.x = 0.0f;
      }
      else if (contact_normal.y > 0.8f) {
        velocity_.y = 0;
        acceleration_.y = 0;
        jump_time_ = 0;
      }
//    }
  }
  
  void PlayerController::Powerup() {
    if (player_state_ == PlayerState::Small) {
      player_state_ = PlayerState::Big;
      
      player_height_ *= 2;
      auto& tc = entity_.GetComponent<TransformComponent>();
      tc.UpdateScale_Y(player_height_);
      
      // Add Impulse to push player out of ground while changing size
      entity_.GetComponent<RigidBodyComponent>().AddVelocity({velocity_.x, 1000.0});
      auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
      pbc.SetSize({0.5f, player_height_ / 2.0f});
      
      jumb_boost_ *= big_jump_boost_factor_;
      walk_speed_ *= big_jump_boost_factor_;
      
      state_machine_->ChangeAction(PlayerAction::PowerUp);
      
      blink_time_ = hurt_invincibility_time_;
    }
    else if (player_state_ == PlayerState::Big) {
      player_state_ = PlayerState::Fire;
      state_machine_->ChangeAction(PlayerAction::PowerUp);
    }
    
    power_up_ = false;
  }
  
  void PlayerController::Die() {
    // State Machine animation
    
    if (player_state_ == PlayerState::Small and blink_time_ <= 0.0f) {
      state_machine_->ChangeAction(PlayerAction::Die);
      
      velocity_ = {0.0, 0.0f};
      acceleration_ = {0.0, 0.0f};
      dead_going_up_ = true;
      is_dead_ = true;

      auto& rb = entity_.GetComponent<RigidBodyComponent>();
      rb.SetVelocity(velocity_);
      rb.is_sensor = true;
      
      const auto& tc = entity_.GetComponent<TransformComponent>();
      dead_max_height_ = tc.Translation().y + 2.0f;
      
      // TODO: Below what lebel
      // if (tc.Translation().y > 0) {
      //  dead_min_height_ = 0.25f;
      // }
      
      // Play sound
    }
    else if (player_state_ == PlayerState::Big) {
      player_state_ = PlayerState::Small;
      
      player_height_ = 1.0f;
      entity_.GetComponent<TransformComponent>().UpdateScale_Y(player_height_);
      jumb_boost_ /= big_jump_boost_factor_;
      walk_speed_ /= big_jump_boost_factor_;
      hurt_invincibility_time_left_ = hurt_invincibility_time_;
      reset_fixture_ = true;

      auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
      pbc.SetSize({0.4f, player_height_ / 2.0f  });

      auto& rb = entity_.GetComponent<RigidBodyComponent>();
      rb.is_sensor = true;

      // Play Sound
    } else if (player_state_ == PlayerState::Fire) {
      player_state_ = PlayerState::Big;

      hurt_invincibility_time_left_ = hurt_invincibility_time_;
      
      // Play Sound
    }
  }
    
  void PlayerController::RenderGui() {
    ImGui::Text(" Acc %f %f", acceleration_.x, acceleration_.y);
    ImGui::Text(" Vel %f %f", velocity_.x, velocity_.y);
    ImGui::Text(" On Ground %d", on_ground_);
    
    ImGui::Text(" Jump Time %d", jump_time_);
    ImGui::Text(" Ground Debouce %f", ground_debounce_);
    ImGui::Text(" Ground Debounce time %f", ground_debounce_time_);
    ImGui::Text(" Jump Boost %f", jumb_boost_);
    
    if(state_machine_)
      ImGui::Text(" State %d", state_machine_->GetAction());
    ImGui::Text(" Power time %f", freez_time_);
    
    ImGui::Text(" Blink Time %f", blink_time_);
    ImGui::Text(" Color A %f", entity_.GetComponent<QuadComponent>().color.a);
    
  }
  
}

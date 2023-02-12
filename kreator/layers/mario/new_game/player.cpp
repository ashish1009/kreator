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
        static const auto& sprites = SpriteManager::GetPlayerStateSprite(state_);
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

    state_machine_ = new StateMachine(&entity_);

    rigid_body_comp_ = &(GetComponent<RigidBodyComponent>());
    rigid_body_comp_->SetGravityScale(0.0f);

    auto& pbc = GetComponent<PillBoxCollider>();
    pbc.bcc.runtime_fixture = (void*)&entity_;
    pbc.top_ccc.runtime_fixture = (void*)&entity_;
    pbc.bottom_ccc.runtime_fixture = (void*)&entity_;
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

    CheckOnGround();
 
    if (Input::IsKeyPressed(KeyCode::Space) and (jump_time_ > 0 or on_ground_ or ground_debounce_ > 0)) {
      if ((on_ground_ or ground_debounce_ > 0) and jump_time_ == 0) { // Just Press Jump Key
        // Play Sound
        jump_time_ = 70;
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
    else if (!on_ground_) {
      if (jump_time_ > 0) {
        velocity_.y *= 0.35f;
        jump_time_ = 0;
      }
      ground_debounce_ -= ts;
      acceleration_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * 2.7f;
    }
    else {
      velocity_.y = 0;
      acceleration_.y = 0;
      ground_debounce_ = ground_debounce_time_;
    }
     
    velocity_.x += acceleration_.x * ts;
    velocity_.y += acceleration_.y * ts;
    
    velocity_.x = std::max(std::min(velocity_.x, terminal_velocity_.x), -terminal_velocity_.x);
    velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
   
    rigid_body_comp_->SetVelocity(velocity_);
    rigid_body_comp_->SetAngularVelocity(0.0f);
    
    if (!on_ground_) {
      state_machine_->ChangeState(PlayerState::Jump);
    }
    else {
//      state_machine_->ChangeState(PlayerState::Idle);
    }
  }
  
  void PlayerController::CheckOnGround() {
    // TODO: Create main API in EnttScene and use wrapper here only
    const auto& tc = entity_.GetComponent<TransformComponent>();
    float inner_player_width = player_width_ * 0.6f;
    float y_val = -(player_height_ / 2 + 0.15f);

    glm::vec2 ray_cast_1_begin = { tc.Translation().x, tc.Translation().y };
    ray_cast_1_begin -= glm::vec2(inner_player_width / 2.0f, 0.0f);
    
    glm::vec2 ray_cast_1_end = ray_cast_1_begin + glm::vec2(0.0f, y_val);
    std::shared_ptr<RayCastInfo> info_1 = entity_.GetScene()->RayCast(&entity_,
                                                                      {ray_cast_1_begin.x, ray_cast_1_begin.y},
                                                                      {ray_cast_1_end.x, ray_cast_1_end.y});
    
    
    glm::vec2 ray_cast_2_begin = ray_cast_1_begin + glm::vec2(inner_player_width, 0.0f);
    glm::vec2 ray_cast_2_end = ray_cast_1_end + glm::vec2(inner_player_width, 0.0f);

    std::shared_ptr<RayCastInfo> info_2 = entity_.GetScene()->RayCast(&entity_,
                                                                      {ray_cast_2_begin.x, ray_cast_2_begin.y},
                                                                      {ray_cast_2_end.x, ray_cast_2_end.y});
    
    on_ground_ = ((info_1->hit and info_1->hit_object and info_1->hit_object->HasComponent<RigidBodyComponent>()) or
                  (info_2->hit and info_2->hit_object and info_2->hit_object->HasComponent<RigidBodyComponent>()));

#if DEBUG_DRAW
    const auto& cd = entity_.GetScene()->GetPrimaryCameraData();
    BatchRenderer::BeginBatch(cd.scene_camera->GetProjection() * glm::inverse(cd.transform_matrix));
    BatchRenderer::DrawLine(glm::vec3(ray_cast_1_begin, 0.0f), glm::vec3(ray_cast_1_end, 0.0f), {0, 1, 0, 1});
    BatchRenderer::DrawLine(glm::vec3(ray_cast_2_begin, 0.0f), glm::vec3(ray_cast_2_end, 0.0f), {0, 1, 0, 1});
    BatchRenderer::EndBatch();
#endif
  }
  
  void PlayerController::RenderGui() {
    ImGui::Text(" Acc %f %f", acceleration_.x, acceleration_.y);
    ImGui::Text(" Vel %f %f", acceleration_.x, acceleration_.y);
    ImGui::Text(" On Ground %d", on_ground_);
    
    ImGui::Text(" Jump Time %d", jump_time_);
    ImGui::Text(" Ground Debouce %f", ground_debounce_);
    ImGui::Text(" Ground Debounce time %f", ground_debounce_time_);
    ImGui::Text(" Jump Boost %f", jumb_boost_);
    
  }
  
}

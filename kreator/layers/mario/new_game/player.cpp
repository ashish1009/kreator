//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#include "player.hpp"

namespace mario {
  
  PlayerController::PlayerController(RigidBodyComponent* rb) : rigid_body_comp_(rb) {
    rigid_body_comp_->SetGravityScale(0.0f);
  }
  
  void PlayerController::Update(Timestep ts) {
    auto& tc = GetComponent<TransformComponent>();
    
    if (Input::IsKeyPressed(KeyCode::Left)) {
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
    
    velocity_.x += acceleration_.x * ts;
    velocity_.y += acceleration_.y * ts;
    
    velocity_.x = std::max(std::min(velocity_.x, terminal_velocity_.x), -terminal_velocity_.x);
    velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
   
    rigid_body_comp_->SetVelocity(velocity_);
    rigid_body_comp_->SetAngularVelocity(0.0f);
  }
  
  void Player::Init(std::shared_ptr<EnttScene> scene) {
    scene_ = scene;
        
    // Hack to add Player first time
    bool create_player = false;
    if (create_player) {
      entity_ = scene_->CreateEntity("Player");
      
      entity_.GetComponent<TransformComponent>().UpdateTranslation({-15, 4, 0.1});
      
      // Quad Component
      auto& qc = entity_.AddComponent<QuadComponent>();
      
      qc.texture_comp.use = true;
      qc.texture_comp.use_sprite = true;
      qc.texture_comp.linear_edge = false;
      
      qc.texture_comp.component = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), qc.texture_comp.linear_edge);
      qc.texture_comp.sprite = SubTexture::CreateFromCoords(qc.texture_comp.component, {6.0f, 30.0f});
      
      // Rigid Body Component
      auto& rbc = entity_.AddComponent<RigidBodyComponent>();
      rbc.type = b2_dynamicBody;
      
      // Box Collider
      auto& pbc = entity_.AddComponent<PillBoxCollider>();
      pbc.width = 0.4f;
      pbc.height = 0.5f;
      
      // Animation
      auto& ac = entity_.AddComponent<AnimationComponent>(qc.texture_comp.component);
      ac.animation = false;
      ac.is_sprite = true;
      ac.sprites.push_back(SubTexture::CreateFromCoords(qc.texture_comp.component, {0.0f, 30.0f}));
      ac.sprites.push_back(SubTexture::CreateFromCoords(qc.texture_comp.component, {1.0f, 30.0f}));
      ac.sprites.push_back(SubTexture::CreateFromCoords(qc.texture_comp.component, {2.0f, 30.0f}));
      
      // Native script
      entity_.AddComponent<NativeScriptComponent>();
    }
    else {
      auto view = scene_->GetEntitesWith<TagComponent>();
      for (auto e : view) {
        auto t = view.get<TagComponent>(e).tag;
        if (t == "Player") {
          entity_ = Entity(e, scene_.get());
          break;
        }
      }
      
      if (entity_.HasComponent<NativeScriptComponent>()) {
        auto& ns = entity_.GetComponent<NativeScriptComponent>();
        ns.Bind<PlayerController>(&entity_.GetComponent<RigidBodyComponent>());
      }
    }
  }
}

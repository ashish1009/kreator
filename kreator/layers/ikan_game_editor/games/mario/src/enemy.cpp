//
//  enemy.cpp
//  kreator
//
//  Created by Ashish . on 16/02/23.
//

#include "enemy.hpp"
#include "player.hpp"
#include "sprite_manager.hpp"

namespace mario {
  
  void EnemyController::Create(Entity entity) {
    entity_ = entity;
    
    rigid_body_comp_ = &(GetComponent<RigidBodyComponent>());
    rigid_body_comp_->SetGravityScale(0.0f);
    
    acceleration_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * 0.5;
  }
  void EnemyController::Update(Timestep ts) {
    // Camera check
    
    if (is_dead_) {
      EnttScene::ResetFixture(rigid_body_comp_->runtime_body);
      
      time_to_kill -= ts;
      rigid_body_comp_->SetVelocity({0., 0});
      if (time_to_kill <= 0) {
        entity_.GetScene()->DestroyEntity(entity_);
      }
      return;
    }
    
    CheckOnGround();
    if (on_ground_) {
      acceleration_.y = 0;
      velocity_.y = 0;
    } else {
      velocity_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * free_fall_factor;
    }
    
    if (going_right_) {
      velocity_.x = walk_speed_;
    }
    else if (!going_right_) {
      velocity_.x = -walk_speed_;
    }
    
    velocity_.y += acceleration_.y * ts * 2.0f;
    
    velocity_.x = std::max(std::min(velocity_.x, terminal_velocity_.x), -terminal_velocity_.x);
    velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
    
    rigid_body_comp_->SetVelocity(velocity_);
    rigid_body_comp_->SetAngularVelocity(0.0f);
  }
  
  void EnemyController::PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    if (is_dead_) {
      return;
    }
    
    if (PlayerController* pc = PlayerController::Get();
        collided_entity->HasComponent<NativeScriptComponent>() and
        collided_entity->GetComponent<NativeScriptComponent>().script.get() == pc) {
      if (!pc->IsDead() && !pc->IsHurt() && contact_normal.y > 0.58f) {
        pc->EnemyBounce();
        stomp();
        contact->SetEnabled(false);
      } else if (!pc->IsDead() && !pc->IsHurt()) {
        //        playerController.die();
        //        if (!playerController.isDead()) {
        //          contact.setEnabled(false);
        //        }
      } else if (!pc->IsDead() && pc->IsHurt()) {
        //        contact.setEnabled(false);
      }
    }
    
    else if (std::abs(contact_normal.y) < 0.1f) {
      going_right_ = contact_normal.x < 0.0f;
    }
  }
  
  void EnemyController::stomp() {
    is_dead_ = true;
    velocity_ = {0.0f, 0.0f};
    rigid_body_comp_->SetVelocity(velocity_);
    rigid_body_comp_->SetAngularVelocity(0.0f);
    rigid_body_comp_->is_sensor = true;
    
    entity_.GetComponent<AnimationComponent>().ClearSprites();
    entity_.RemoveComponent<AnimationComponent>();
    
    auto& qc = entity_.GetComponent<QuadComponent>();
    qc.texture_comp.sprite = SpriteManager::GetEnemySprite(type_, EnemyState::Dead);
    
    // Play Sound
  }
  
  void EnemyController::CheckOnGround() {
    static float inner_player_width = 0.6f;
    static float y_val = -0.52;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
  }
  
  void EnemyController::RenderGui() {
    ImGui::Text(" Acc %f %f", acceleration_.x, acceleration_.y);
    ImGui::Text(" Vel %f %f", velocity_.x, velocity_.y);
    ImGui::Text(" On Ground %d", on_ground_);
  }
  
  struct EnemyScriptData {
    std::unordered_map<EnemyType, EnemyData> script_map;
  };
  static EnemyScriptData* data;
  
  const EnemyData& EnemyScriptManager::GetData(EnemyType type) {
    return data->script_map.at(type);
  }
  void EnemyScriptManager::Init() {
    data = new EnemyScriptData();
    
    static auto goomba_load_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::EnemyController") {
        sc->Bind<mario::EnemyController>(EnemyType::Goomba);
        return true;
      }
      return false;
    };
    
    data->script_map[EnemyType::Goomba] = {EnemyType::Goomba, goomba_load_fn};
  }
  void EnemyScriptManager::Shutdown() {
    delete data;
  }
  
}

//
//  enemy.cpp
//  kreator
//
//  Created by Ashish . on 14/02/23.
//

#include "enemy.hpp"
#include "player.hpp"

namespace mario {
  
  void GoombaController::Create(Entity entity) {
    entity_ = entity;

    rigid_body_comp_ = &(GetComponent<RigidBodyComponent>());
    rigid_body_comp_->SetGravityScale(0.0f);
    
    acceleration_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * 0.5;
  }
  void GoombaController::Update(Timestep ts) {
    // Camera check
    
    if (is_dead_) {
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
  void GoombaController::PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    if (is_dead_) {
      return;
    }

    
    if (PlayerController* pc = PlayerController::Get();
        collided_entity->HasComponent<NativeScriptComponent>() and
        collided_entity->GetComponent<NativeScriptComponent>().script.get() == pc) {
      if (!pc->IsDead() && !pc->IsHurt() && contact_normal.y > 0.58f) {
        pc->EnemyBounce();
        //        stomp();
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
  
  void GoombaController::CheckOnGround() {
    static float inner_player_width = 0.6f;
    static float y_val = -0.52;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
  }
  
  void GoombaController::RenderGui() {
    ImGui::Text(" Acc %f %f", acceleration_.x, acceleration_.y);
    ImGui::Text(" Vel %f %f", velocity_.x, velocity_.y);
    ImGui::Text(" On Ground %d", on_ground_);
  }


  struct EnemyData {
    ScriptLoaderFn loader_fun;
    EnemyData() = default;
    EnemyData(ScriptLoaderFn fun) : loader_fun(fun) { }
  };
  
  struct EnemyScriptData {
    std::unordered_map<EnemyType, EnemyData> script_map;
  };
  static EnemyScriptData* data;

  ScriptLoaderFn EnemyScriptManager::GetLoaderFn(EnemyType type) {
    return data->script_map.at(type).loader_fun;
  }
  void EnemyScriptManager::Init() {
    data = new EnemyScriptData();
    data->script_map[EnemyType::Goomba].loader_fun = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::GoombaController") {
        sc->Bind<mario::GoombaController>();
        return true;
      }
      return false;
    };
  }
  void EnemyScriptManager::Shutdown() {
    delete data;
  }
  
}

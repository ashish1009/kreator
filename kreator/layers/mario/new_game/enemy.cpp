//
//  enemy.cpp
//  kreator
//
//  Created by Ashish . on 14/02/23.
//

#include "enemy.hpp"

namespace mario {
  
  void GoombaController::Create(Entity entity) {
    entity_ = entity;

    rigid_body_comp_ = &(GetComponent<RigidBodyComponent>());
    rigid_body_comp_->SetGravityScale(0.0f);
    
    acceleration_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * 2.7f;
  }
  void GoombaController::Update(Timestep ts) {
    // Camera check
    
    if (is_dead_) {
    }
    
    CheckOnGround();

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
//    LivingEntityHitCheck(collided_entity, contact);
    if (std::abs(contact_normal.y) < 0.1f) {
      going_right_ = contact_normal.x < 0.0f;
    }
  }
  
  void GoombaController::CheckOnGround() {
    static float inner_player_width = 0.6f;
    static float y_val = -0.52;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
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

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
    velocity_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * 3.7f;
    rigid_body_comp_ = &(entity_.GetComponent<RigidBodyComponent>());

  }
  void GoombaController::Update(Timestep) {
    if (going_right_ and std::abs(rigid_body_comp_->velocity.x) < max_speed_) {
      rigid_body_comp_->AddVelocity(velocity_);
    }
    else if (!going_right_ and std::abs(rigid_body_comp_->velocity.x) < max_speed_) {
      rigid_body_comp_->AddVelocity({-velocity_.x, velocity_.y});
    }
  }
  void GoombaController::PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
//    LivingEntityHitCheck(collided_entity, contact);
    if (std::abs(contact_normal.y) < 0.1f) {
      going_right_ = contact_normal.x < 0.0f;
    }
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

//
//  enemy.cpp
//  kreator
//
//  Created by Ashish . on 11/03/23.
//

#include "enemy.hpp"
#include "player.hpp"
#include "sprite_manager.hpp"

namespace mario {
  
  void EnemyController::Create(Entity entity) {
    entity_ = entity;
    
    // Disbale Gravity on Enemy
    GetComponent<RigidBodyComponent>().SetGravityScale(0.0f);

    // Free fall with scene gravity
    acceleration_.y = entity_.GetScene()->GetGravity().y * free_fall_factor;
  }

  void EnemyController::Update(Timestep ts) {
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    
    if (reset_fixture_) {
      const auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
      EnttScene::ResetPillBoxColliderFixture(entity_.GetComponent<TransformComponent>(), &rb, pbc);
      reset_fixture_ = false;
    }
    
    // Change the direction of duck. No need for Goomba
    if (type_ == EnemyType::Duck) {
      auto& tc = entity_.GetComponent<TransformComponent>();
      tc.UpdateScale_X(going_right_ ? -1.0f : 1.0f);
    }

    CheckOnGround();

    if (on_ground_) {
      acceleration_.y = 0;
      velocity_.y = 0;
    } else {
      acceleration_.y = entity_.GetScene()->GetGravity().y * free_fall_factor;
    }

    if (!is_dying_ and !is_dead_) {
      if (going_right_) {
        velocity_.x = walk_speed_;
      }
      else if (!going_right_) {
        velocity_.x = -walk_speed_;
      }
    }
    else {
      velocity_.x = 0.0f;
    }
    
    velocity_.y += acceleration_.y * ts * 2.0f;
    
    velocity_.x = std::max(std::min(velocity_.x, terminal_velocity_.x), -terminal_velocity_.x);
    velocity_.y = std::max(std::min(velocity_.y, terminal_velocity_.y), -terminal_velocity_.y);
    
    rb.SetVelocity(velocity_);
    rb.SetAngularVelocity(0.0f);
  }
  
  void EnemyController::PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    if (is_dead_) {
      return;
    }

    if (std::abs(contact_normal.y) < 0.1f) {
      going_right_ = contact_normal.x < 0.0f;
    }
  }
  
  void EnemyController::Copy(void* script) {
    if (!script)
      return;
    
    EnemyController* enemy_script = reinterpret_cast<EnemyController*>(script);
    IK_ASSERT(enemy_script);
    
    type_ = enemy_script->type_;
    
    is_dead_ = enemy_script->is_dead_;
    going_right_ = enemy_script->going_right_;
    on_ground_ = enemy_script->on_ground_;
    
    free_fall_factor = enemy_script->free_fall_factor;
    walk_speed_ = enemy_script->walk_speed_;
    time_to_kill = enemy_script->time_to_kill;
    
    acceleration_ = enemy_script->acceleration_;
    velocity_ = enemy_script->velocity_;
    terminal_velocity_ = enemy_script->terminal_velocity_;
  }
  
  void EnemyController::RenderGui() {
  }
 
  void EnemyController::CheckOnGround() {
    static float inner_player_width = 0.6f;
    static float y_val = -0.52;
    
    on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
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
    
    static auto enemy_load_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::EnemyController") {
        sc->Bind<mario::EnemyController>(EnemyType::Goomba);
        return true;
      }
      return false;
    };
    
    data->script_map[EnemyType::Goomba] = {EnemyType::Goomba, enemy_load_fn};
    data->script_map[EnemyType::Duck] = {EnemyType::Duck, enemy_load_fn};
  }
  void EnemyScriptManager::Shutdown() {
    delete data;
  }

}

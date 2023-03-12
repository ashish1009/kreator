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
    
    // Change the direction of turtule. No need for Goomba
    if (type_ == EnemyType::Turtule) {
      auto& tc = entity_.GetComponent<TransformComponent>();
      tc.UpdateScale_X(going_right_ ? -1.0f : 1.0f);
    }
  }

  void EnemyController::Update(Timestep ts) {
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    
    if (reset_fixture_) {
      if (type_ == EnemyType::Goomba) {
        const auto& cc = entity_.GetComponent<CircleColliiderComponent>();
        EnttScene::ResetCircleColliderFixture(entity_.GetComponent<TransformComponent>(), &rb, cc);
      }
      else if (type_ == EnemyType::Turtule) {
        const auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
        EnttScene::ResetPillBoxColliderFixture(entity_.GetComponent<TransformComponent>(), &rb, pbc);
      }
      reset_fixture_ = false;
    }
        
    if (die_animation_) {
      die_animation_time_ -= ts;
      if (die_animation_time_ > 0.8f) {
        rb.AddVelocity({1.0f, 20.0f});
      }
      else if (die_animation_time_ <= 0.8f and die_animation_time_ > 0.0f) {
        rb.AddVelocity({0.0f, -20.0f});
      }
      else if (die_animation_time_ <= 0.0f) {
        entity_.GetScene()->DestroyEntity(entity_);
        die_animation_ = false;
      }
      return;
    }
    
    // Destory the entity if enemy is dead after stomp
    if (is_dead_) {
      time_to_kill -= ts;
      rb.SetVelocity({0., 0});
      if (time_to_kill <= 0) {
        entity_.GetScene()->DestroyEntity(entity_);
      }
      return;
    }
    
    // If Turtule is dying then make it stationary
    if (is_dying_ and !force_applied_) {
      if (type_ == EnemyType::Turtule) {
        time_to_revive_ -= ts;
        
        // Revive the turtule if not killed properly
        if (time_to_revive_ <= 1.0f and time_to_revive_ >= 0.0f) {
          auto& ac = entity_.GetComponent<AnimationComponent>();
          ac.sprites = SpriteManager::GetEnemySprite(EnemyType::Turtule, EnemyState::Dying);
          ac.speed = 15;
          ac.animation = true;
        }
        else if (time_to_revive_ <= 0.0f) {
          entity_.GetComponent<TransformComponent>().UpdateScale_Y(2.0f);
          auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
          pbc.height = 0.8f;
          pbc.offset.y = -0.20f;
          pbc.RecalculateColliders();
          
          auto& ac = entity_.GetComponent<AnimationComponent>();
          ac.sprites = SpriteManager::GetEnemySprite(EnemyType::Turtule, EnemyState::Alive);
          ac.speed = 50.0f;
          ac.animation = true;
          
          // Add Impulse to push e out of ground while changing size
          entity_.GetComponent<RigidBodyComponent>().AddVelocity({0, 1000.0});
          
          SetAppliedForce(false);
          is_dying_ = false;
          reset_fixture_ = true;
        }
      }
    }

    CheckOnGround();

    if (on_ground_) {
      acceleration_.y = 0;
      velocity_.y = 0;
    } else {
      acceleration_.y = entity_.GetScene()->GetGravity().y * free_fall_factor;
    }
    
    if (!is_dying_ or (is_dying_ and force_applied_)) {
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
    
    if (collided_entity->HasComponent<BulletComponent>()) {
      Die();
    }

    if (PlayerController* pc = PlayerController::Get();
        collided_entity->HasComponent<NativeScriptComponent>() and
        collided_entity->GetComponent<NativeScriptComponent>().script.get() == pc) {
            
      if (!pc->IsDying() and !pc->IsHurtInvincible() and contact_normal.y > 0.58f) {
        pc->EnemyBounce();
        Stomp();
        contact->SetEnabled(false);
      } else if (!pc->IsDying() and !pc->IsInvincible()) {
        // Apply force to turtule if is dying
        if (is_dying_) {
          going_right_ = (contact_normal.x < 0);
          SetAppliedForce(true);
        }
#if 0
        pc->Die();
        if (!pc->IsDying()) {
          contact->SetEnabled(false);
        }
#endif
      }
    }
    
    else if (std::abs(contact_normal.y) < 0.1f) {
      going_right_ = contact_normal.x < 0.0f;
      
      // Change the direction of turtule. No need for Goomba
      if (type_ == EnemyType::Turtule) {
        auto& tc = entity_.GetComponent<TransformComponent>();
        tc.UpdateScale_X(going_right_ ? -1.0f : 1.0f);
      }
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
    
    force_applied_ = enemy_script->force_applied_;
    die_animation_ = enemy_script->die_animation_;
    die_animation_time_ = enemy_script->die_animation_time_;
  }
  
  void EnemyController::RenderGui() {
  }
  
  void EnemyController::SetAppliedForce(bool force) {
    force_applied_ = force;
    is_deadly_ = force;
    if (force) {
      walk_speed_ = 8.0f;
      if (!entity_.HasComponent<BulletComponent>())
        entity_.AddComponent<BulletComponent>();
    }
    else {
      walk_speed_ = 4.0f;
      if (entity_.HasComponent<BulletComponent>())
        entity_.RemoveComponent<BulletComponent>();
    }
  }
  
  void EnemyController::Stomp() {
    reset_fixture_ = true;

    auto& qc = entity_.GetComponent<QuadComponent>();
    if (type_ == EnemyType::Turtule) {
      entity_.GetComponent<TransformComponent>().UpdateScale_Y(1.0f);
      auto& pbc = entity_.GetComponent<PillBoxColliderComponent>();
      pbc.height = 0.5f;
      pbc.offset.y = 0.0f;
      pbc.RecalculateColliders();
      
      is_dying_ = true;
      time_to_revive_ = 5.0f;
      SetAppliedForce(false);
      
      entity_.GetComponent<AnimationComponent>().animation = false;
      qc.texture_comp.sprite = SpriteManager::GetEnemySprite(type_, EnemyState::Dying).at(0);
      return;
    }
    
    is_dead_ = true;
    velocity_ = {0.0f, 0.0f};
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    rb.SetVelocity(velocity_);
    rb.SetAngularVelocity(0.0f);
    rb.is_sensor = true;
    
    entity_.RemoveComponent<AnimationComponent>();
    
    qc.texture_comp.sprite = SpriteManager::GetEnemySprite(type_, EnemyState::Dead).at(0);
    
    const auto& tc = entity_.GetComponent<TransformComponent>();
    RuntimeItem::Create(Items::Score, entity_.GetScene(), {tc.Translation().x - 0.5, tc.Translation().y + 1}, 200);
  }
 
  void EnemyController::Die() {
    if (type_ == EnemyType::Turtule) {
      entity_.GetComponent<TransformComponent>().UpdateScale_Y(1.0f);
      entity_.GetComponent<QuadComponent>().texture_comp.sprite = SpriteManager::GetEnemySprite(type_, EnemyState::Dying).at(0);
    }

    entity_.GetComponent<AnimationComponent>().animation = false;
    
    auto& tc = entity_.GetComponent<TransformComponent>();
    tc.UpdateScale_Y(tc.Scale().y * -1.0f);
    
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    rb.is_sensor = true;
    
    die_animation_ = true;
    reset_fixture_ = true;
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
    data->script_map[EnemyType::Turtule] = {EnemyType::Turtule, enemy_load_fn};
  }
  void EnemyScriptManager::Shutdown() {
    delete data;
  }

}

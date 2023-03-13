//
//  enemy.hpp
//  kreator
//
//  Created by Ashish . on 11/03/23.
//

#pragma once

#include "runtime_item.hpp"

namespace mario {
  
  using namespace ikan;
  
  enum class EnemyType {
    Goomba, Turtule
  };
  
  enum class EnemyState {
    Alive, Dying, Dead
  };

  class EnemyController : public ScriptableEntity {
  public:
    EnemyController(EnemyType type) { type_ = type; }

    void Create(Entity entity) override;
    void Update(Timestep) override;
    void PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) override;
    void RenderGui() override;
    void Copy(void* script) override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(EnemyController);

  private:
    void Die();
    void Stomp();
    void CheckOnGround();
    void SetAppliedForce(bool force);

    EnemyType type_;
    
    bool reset_fixture_ = false;
    
    bool is_dead_ = false;
    bool is_dying_ = false;
    bool going_right_ = true;
    bool on_ground_ = false;
    bool force_applied_ = false;
    bool is_deadly_ = false;
    bool die_animation_ = false;
    
    float free_fall_factor = 1.0f;
    float walk_speed_ = 4.0f;
    float time_to_kill = 0.5f;
    float time_to_revive_ = 1.5f;
    float die_animation_time_ = 1.0f;

    glm::vec2 acceleration_;
    glm::vec2 velocity_;
    glm::vec2 terminal_velocity_ = {8.1f, 18.1f};
  };
  
  struct EnemyData {
    EnemyType type;
    ScriptLoaderFn loader_fun;
    EnemyData() = default;
    EnemyData(EnemyType type, ScriptLoaderFn fun) : type(type), loader_fun(fun) { }
  };
  
  class EnemyScriptManager {
  public:
    static void Init();
    static void Shutdown();
    
    static const EnemyData& GetData(EnemyType type);
  };
  using ESM = EnemyScriptManager;
  
  
  inline bool IsEnemy(const std::string& tag) {
    return tag == "Goomba" or tag == "Turtule";
  }
  
  inline EnemyType GetType(const std::string& tag) {
    if (tag == "Goomba") return EnemyType::Goomba;
    if (tag == "Turtule") return EnemyType::Turtule;
    return EnemyType::Goomba;
  }

}
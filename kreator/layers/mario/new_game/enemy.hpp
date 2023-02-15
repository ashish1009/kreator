//
//  enemy.hpp
//  kreator
//
//  Created by Ashish . on 14/02/23.
//

#pragma once

#include "runtime_items.hpp"

namespace mario {
  
  using namespace ikan;
  
  enum class EnemyType {
    Goomba
  };
  
  class GoombaController : public ScriptableEntity {
  public:
    void Create(Entity entity) override;
    void Update(Timestep) override;
    void PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) override;

  private:
    void CheckOnGround();

    bool is_dead_ = false;
    bool going_right_ = true;
    bool on_ground_ = false;

    float walk_speed_ = 4.0f;
    float time_to_kill = 0.5f;

    glm::vec2 acceleration_;
    glm::vec2 velocity_;
    glm::vec2 terminal_velocity_ = {8.1f, 18.1f};

    RigidBodyComponent* rigid_body_comp_;
  };
  
  class EnemyScriptManager {
  public:
    static void Init();
    static void Shutdown();
    
    static ScriptLoaderFn GetLoaderFn(EnemyType type);
  };
  using ESM = EnemyScriptManager;

  
  inline bool IsEnemy(const std::string& tag) {
    return tag == "Goomba";
  }

}

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
    RigidBodyComponent* rigid_body_comp_;
    glm::vec2 velocity_ = { 4.0f, 0.0f };
    float max_speed_ = 8.0f;
    bool going_right_ = true;
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

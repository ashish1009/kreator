//
//  brick_script.h
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

#include "player.hpp"

namespace mario {
  
  class BlockController : public ScriptableEntity {
  public:
    enum class Type {
      Empty, Coin, PowerUp, Star 
    };
    
    BlockController(Type type);
    ~BlockController() = default;
    
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void BeginCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 contact_normal) override;
    void EndCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PreSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PostSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void RenderGui() override {}
    
    void SetInactive() { active_ = false; }

  private:
    void PlayerHit(PlayerController* pc);
    
    bool going_up_ = true;
    bool animation_ = false;
    glm::vec2 start_pos_;
    glm::vec2 end_pos_;
    
    bool active_ = true;
    float speed_ = 3.0f;
    
    Type type_ = Type::Empty;
  };
  
  class BlockScriptManager {
  public:
    static void Init();
    static void Shutdown();
    
    static ScriptLoaderFn GetLoaderFn(const std::string& tag);
    static BlockController::Type GetType(const std::string& tag);
  };  
  
}

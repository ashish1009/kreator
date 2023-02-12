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
      Coin, PowerUp, Star, Empty
    };
    
    BlockController(Type type) : type_(type) {
      
    }
    
    ~BlockController() = default;
    
    void Create(Entity entity) override {
      entity_ = entity;
      start_pos_ = glm::vec2(entity_.GetComponent<TransformComponent>().Translation());
      end_pos_ = start_pos_ + glm::vec2(0.0f, 0.4f);
    }
    
    void Update(Timestep ts) override {
      auto& tc = entity_.GetComponent<TransformComponent>();
      if (animation_) {
        if (going_up_) {
          if (tc.Translation().y < end_pos_.y) {
            tc.UpdateTranslation_Y(tc.Translation().y + (ts * speed_));
          }
          else {
            going_up_ = false;
          }
        }
        else {
          if (tc.Translation().y > start_pos_.y) {
            tc.UpdateTranslation_Y(tc.Translation().y - (ts * speed_));
          }
          else {
            tc.UpdateTranslation_Y(start_pos_.y);
            going_up_ = true;
            animation_ = false;
          }
        }
      }
    }
    
    void BeginCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 contact_normal) override {
      PlayerController* pc = PlayerController::Get();
      if (active_ and pc and contact_normal.y < -0.8f) {
        animation_ = true;
        PlayerHit(pc);
        // ADD Suund
      }
    }
    
    void EndCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PreSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PostSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void RenderGui() override {}
    
    void SetInactive() {
      active_ = false;
    }

  private:
    void PlayerHit(PlayerController* pc) {
      if (!pc->IsSmall()) {
        // Play sound // Break
        // Destory the object
      }
    }
    
    bool going_up_ = true;
    bool animation_ = false;
    glm::vec2 start_pos_;
    glm::vec2 end_pos_;
    
    bool active_ = true;
    float speed_ = 3.0f;
    
    Type type_ = Type::Empty;
  };
  
}

//
//  brick_script.h
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace mario {

  class BlockController : public ScriptableEntity {
  public:
    BlockController() = default;
    ~BlockController() = default;
    
    void Create(Entity entity) override {
      entity_ = entity;
      start_pos_ = glm::vec2(entity_.GetComponent<TransformComponent>().Translation());
      end_pos_ = start_pos_ += glm::vec2(0.0f, 0.4f);
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
    
    void BeginCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {
      
    }
    
    void EndCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PreSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PostSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void RenderGui() override {}

  private:
    bool going_up_ = true;
    bool animation_ = true;
    glm::vec2 start_pos_;
    glm::vec2 end_pos_;
    
    bool active_ = true;
    float speed_ = 0.4f;
  };
  
}

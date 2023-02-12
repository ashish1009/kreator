//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#pragma once

#include "enums.h"
#include "common.h"

namespace mario {
  
  using namespace ikan;
  
  class StateMachine {
  public:
    StateMachine(Entity* entity) {
      player_entity_ = entity;
    }
    
    void Update();
    
    void ChangeState(PlayerState state);
    PlayerState GetState() const { return state_;}
    
  private:
    PlayerState state_ = PlayerState::Idle;
    Entity* player_entity_;
  };
  
  class PlayerController : public ScriptableEntity {
  public:
    PlayerController();
    ~PlayerController();

    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void RenderGui() override;
    void BeginCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override;
    void EndCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PreSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PostSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    
    bool IsSmall() const { return player_size == PlayerSize::Small; }
    
    static PlayerController* Get() { return instance_; }

  private:
    void CheckOnGround();

    float warlk_speed_ = 4.0f;
    float slow_down_force_ = 0.08f;
    glm::vec2 terminal_velocity_ = {8.1f, 8.1f};
    
    bool on_ground_ = false;
    float ground_debounce_ = 0.0f; // Seconds
    float ground_debounce_time_ = 0.1f; // Seconds
    
    float player_width_ = 1.0f;
    float player_height_ = 1.0f;

    float jumb_boost_ = 1.0f;
    float big_jump_boost_ = 1.0f;
    float jump_impuls_ = 3.0f;
    int32_t jump_time_ = 0;
    
    glm::vec2 acceleration_;
    glm::vec2 velocity_;
    bool is_dead_ = false;
    int32_t enemy_bounce_ = 0;
    
    PlayerSize player_size = PlayerSize::Small;
    RigidBodyComponent* rigid_body_comp_;
    StateMachine* state_machine_;
    
    static PlayerController* instance_;
  };
  
}

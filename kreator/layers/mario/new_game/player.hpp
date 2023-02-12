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
    PlayerController() = default;
    ~PlayerController() {
      delete state_machine_;
    }

    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void RenderGui() override;
    
  private:
    void CheckOnGround();
    
    float warlk_speed_ = 10.0f;
    float jumb_boost_ = 1.0f;
    float jump_impuls_ = 3.0f;
    float slow_down_force_ = 0.08f;
    glm::vec2 terminal_velocity_ = {10.1f, 10.1f};
    
    bool on_ground_ = false;
    float ground_debounce_ = 0.0f; // Seconds
    float ground_debounce_time_ = 0.1f; // Seconds
    
    float big_jump_boost_ = 1.0f;
    float player_width_ = 1.0f;
    float player_height_ = 1.0f;
    int32_t jump_time_ = 0;
    
    glm::vec2 acceleration_;
    glm::vec2 velocity_;
    bool is_dead_ = false;
    int32_t enemy_bounce_ = 0;
    
    PlayerSize player_size = PlayerSize::Small;
    RigidBodyComponent* rigid_body_comp_;
    StateMachine* state_machine_;
  };
  
}

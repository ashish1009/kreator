//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;

  enum class PlayerAction {
    Invalid, Idle, Run, SwitchSide, Jump, Die, PowerUp
  };

  enum class PlayerState {
    Invalid, Small, Big, Fire, Invicible
  };
  
  class StateMachine {
  public:
    StateMachine(Entity* entity);

    PlayerState State() const {
      IK_ASSERT(player_state_ != PlayerState::Invalid);
      return player_state_;
    }
    
    PlayerState PrevState() const {
      IK_ASSERT(player_state_ != PlayerState::Invalid);
      return player_prev_state_;
    }
    
    void SetState(PlayerState new_state) {
      player_prev_state_ = player_state_;
      player_state_ = new_state;
    }

    PlayerAction Action() const {
      IK_ASSERT(player_action_ != PlayerAction::Invalid);
      return player_action_;
    }
    
    PlayerAction PrevAction() const {
      IK_ASSERT(player_action_ != PlayerAction::Invalid);
      return player_prev_action_;
    }

    void SetAction(PlayerAction new_action);
    void Update(Timestep ts);
    
    std::string StateString() const;
    std::string ActionString() const;

  private:
    Entity* player_entity_;
    
    PlayerState player_prev_state_ = PlayerState::Invalid;
    PlayerState player_state_ = PlayerState::Small;
    
    PlayerAction player_action_ = PlayerAction::Idle;
    PlayerAction player_prev_action_ = PlayerAction::Invalid;
  };

  class PlayerController : public ScriptableEntity {
  public:
    PlayerController();
    ~PlayerController();
    
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void RenderGui() override;

    static PlayerController* Get();
    
  private:
    void CheckOnGround();
    void SetState(PlayerState new_state);
    
    bool reset_fixture_ = false;
    bool on_ground_ = false;
    
    float player_width_ = 1.0f, player_height_ = 1.0f;;

    float free_fall_factor = 2.7f;

    float walk_speed_ = 4.0f;
    float slow_down_force_ = 0.08f;

    float ground_debounce_ = 0.0f; // Seconds
    float ground_debounce_time_ = 0.1f; // Seconds

    float jumb_boost_ = 1.0f;
    float big_jump_boost_ = 1.0f;
    float jump_impuls_ = 10.0f;
    float big_jump_boost_factor_ = 2.0f;
    
    int32_t enemy_bounce_ = 0;
    int32_t jump_time_ = 0;

    glm::vec2 acceleration_;
    glm::vec2 velocity_;
    glm::vec2 terminal_velocity_ = {8.1f, 18.1f};

    StateMachine* state_machine_;

    static PlayerController* instance_;
  };
  
}

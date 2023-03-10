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
    Invalid, Small, Big, Fire, Invicible, Luigi
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
    void ChangeState(PlayerState new_state);
    
    bool reset_fixture_ = false;
    bool on_ground_ = false;
    
    float player_width_ = 1.0f, player_height_ = 1.0f;;

    glm::vec2 velocity_;

    StateMachine* state_machine_;

    static PlayerController* instance_;
  };
  
}

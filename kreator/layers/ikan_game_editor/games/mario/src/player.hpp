//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  enum class PlayerAction {
    Idle, Run, SwitchSide, Jump,
    Die, PowerUp
  };
  
  enum class PlayerState {
    Small, Big, Fire, Invicible, Luigi
  };
  
  class StateMachine {
  public:
    StateMachine(Entity* entity, PlayerState* state) {
      player_entity_ = entity;
      player_state_ = state;
    }
    
    void Update(Timestep ts);
    
    void ChangeAction(PlayerAction state);
    PlayerAction GetAction() const { return player_action_;}
    PlayerAction GetPrevAction() const { return prev_action_;}
    
  private:
    PlayerAction player_action_ = PlayerAction::Idle;
    PlayerAction prev_action_;
    
    Entity* player_entity_;
    PlayerState* player_state_;
  };
  
  class PlayerController : public ScriptableEntity {
  public:
    PlayerController();
    ~PlayerController();
    
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void RenderGui() override;
    void BeginCollision(Entity* collided_entity, b2Contact* contact, const glm::vec2& constact_normal) override;

    void Die();
    
    void SetPowerup() { power_up_ = true; }
    void EnemyBounce() { enemy_bounce_ = 18; }
    
    bool IsSmall() const { return player_state_ == PlayerState::Small; }
    bool IsDead() const { return is_dead_; }
    bool IsHurtInvincible() const { return hurt_invincibility_time_left_ > 0.0f; }
    bool IsInvincible() const { return player_state_ == PlayerState::Invicible or IsHurtInvincible(); }
    
    static PlayerController* Get() { return instance_; }
    
  private:
    void CheckOnGround();
    void Powerup();
    
    const float a_[2] = {0, 1};

    bool power_up_ = false;
    bool on_ground_ = false;
    bool is_dead_ = false;
    
    bool reset_fixture_ = false;
    bool dead_going_up_ = false;
    
    float hurt_invincibility_time_ = 1.5f;
    float hurt_invincibility_time_left_ = 0.0f;
    float blink_time_ = 1.5f;

    int32_t blink_speed_ = 10;
    int32_t blink_a_idx_ = 0;
    
    float dead_max_height_ = 0.0f;
    float dead_min_height_ = 0.0f;

    float walk_speed_ = 4.0f;
    float slow_down_force_ = 0.08f;
    
    float free_fall_factor = 2.7f;
    
    float ground_debounce_ = 0.0f; // Seconds
    float ground_debounce_time_ = 0.1f; // Seconds
    
    float player_width_ = 1.0f;
    float player_height_ = 1.0f;
    
    float freez_time_ = 0.5f;
    
    float jumb_boost_ = 1.0f;
    float big_jump_boost_ = 1.0f;
    float jump_impuls_ = 10.0f;
    float big_jump_boost_factor_ = 2.0f;
    
    int32_t enemy_bounce_ = 0;
    int32_t jump_time_ = 0;
    
    glm::vec2 acceleration_;
    glm::vec2 velocity_;
    glm::vec2 terminal_velocity_ = {8.1f, 18.1f};

    PlayerState player_state_ = PlayerState::Small;
    StateMachine* state_machine_;

    static PlayerController* instance_;
  };
  
}

//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;

  enum class PlayerState {
    Small, Big, Fire, Invicible, Luigi
  };
  
  class StateMachine {
  public:
    StateMachine(Entity* entity);

    friend class PlayerController;
    
  private:
    Entity* player_entity_;
    PlayerState player_state_ = PlayerState::Small;
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
    
    bool reset_fixture_ = false;
    bool on_ground_ = false;
    
    float player_width_ = 1.0f, player_height_ = 1.0f;;

    StateMachine* state_machine_;

    static PlayerController* instance_;
  };
  
}

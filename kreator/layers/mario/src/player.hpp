//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

#define MARIO_DEBUG 1

namespace mario {
  
  using namespace ikan;
  
  class PlayerController : public ScriptableEntity {
  public:
    enum class State {
      Freefall
    };
    
    /// Default destructor of playr controller
    PlayerController() = default;
    
    /// This function update the player each frame based on the state of player
    /// - Parameter ts: time step
    void Update(Timestep ts) override;
    /// This function Renders Gui for debugging
    void RenderGui() override;
    
  private:
    /// This function trigger for player's freefall
    /// - Parameter ts: time step
    void Freefall(Timestep ts);
    
    // -----------------
    // Member variables
    // -----------------
    State state_ = State::Freefall;
    
    // TODO: Link speed with camera controller speed
    float speed_ = 25.0f;
  };

  class Player {
  public:
    /// Constructor of mario Player
    Player(EnttScene* scene);
    /// Destructor of mario Player
    ~Player();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(Player);
    
#if MARIO_DEBUG
    void RenderGui();
    void Reset();
#endif
    
  private:
    EnttScene* scene_;
    Entity player_entity_;
  };
  
}

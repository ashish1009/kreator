//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

#include "common.h"

namespace mario {
  
  namespace player_data {
    // Speed of player
    static constexpr float speed_ = 60.0f;

    // States of playe
    using State = uint32_t;
#define FreeFalling BIT(1)
  }
  
  using namespace player_data;
  using namespace ikan;
  
  class PlayerController : public ScriptableEntity {
  public:
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
    /// This function checks is the the state in argumet is present in player or not
    /// - Parameter state_bit: state bit
    bool IsState(State state_bit);

#if MARIO_DEBUG
    std::string GetStateString();
#endif
    
    // -----------------
    // Member variables
    // -----------------
    State state_bits_ = FreeFalling;
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

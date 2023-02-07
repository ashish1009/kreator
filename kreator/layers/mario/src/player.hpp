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
  using namespace ikan;
  
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

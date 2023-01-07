//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class Player {
  public:
    /// Constructor of mario Player
    Player(EnttScene* scene);
    /// Destructor of mario Player
    ~Player();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(Player);
    
  private:
    EnttScene* scene_;
    Entity player_entity_;
  };
  
}

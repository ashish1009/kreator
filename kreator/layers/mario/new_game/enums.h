//
//  enums.h
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace mario {
  
  enum class SpriteType {
    Player
  };
  
  enum class PlayerState {
    Idle     = 0,  Run     = 1,   SwitchSide     = 2,   Jump     = 3,   Die = 4,
    BigIdle  = 5,  BigRun  = 6,   BigSwitchSide  = 7,   BigJump  = 8,
    FireIdle = 10, FireRun = 11,  FireSwitchSide = 12,  FireJump = 13,
  };
  
  enum class PlayerSize {
    Small = 0, Big = 1, Fire = 2, Invicible = 4
  };

}

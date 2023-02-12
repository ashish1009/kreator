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
    Idle, Run, SwitchSide, Jump, Die,
    BigIdle, BigRun, BigSwitchSide, BigJump,
    FireIdle, FireRun, FireSwitchSide, FireJump,
  };

}

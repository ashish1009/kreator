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
  
  class PlayerController : public ScriptableEntity {
  public:
  };
  
}

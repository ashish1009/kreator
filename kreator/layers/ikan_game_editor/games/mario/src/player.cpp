//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#include "player.hpp"

namespace mario {

  PlayerController* PlayerController::instance_ = nullptr;
  
  PlayerController::PlayerController() {
    instance_ = this;
  }
  
  PlayerController* PlayerController::Get() {
    return instance_;
  }
  
}
 

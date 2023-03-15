//
//  mario.hpp
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#pragma once

#include "layers/game_data.hpp"

namespace mario {
  
  using namespace ikan;
  
  class Mario : public kreator::GameData {
  public:
    std::string GameName() const override { return "ikan_mario"; }
  };
  
}

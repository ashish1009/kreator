//
//  game_data.cpp
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#include "game_data.hpp"

namespace kreator {
  
  std::unique_ptr<GameData> CreateGameData(GameType game_type) {
    return std::make_unique<GameData>();
  }
  
}

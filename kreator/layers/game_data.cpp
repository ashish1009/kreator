//
//  game_data.cpp
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#include "game_data.hpp"
#include "mario_main.hpp"

namespace kreator {
  
  std::unique_ptr<GameData> CreateGameData(GameType game_type) {
    switch (game_type) {
      case GameType::Mario: return std::make_unique<mario::Mario>();
      case GameType::None:
      default:
        return std::make_unique<GameDataImpl>();
    }
  }
  
}

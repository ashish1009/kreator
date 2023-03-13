//
//  game_data.h
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#pragma once

namespace kreator {
  
  using namespace ikan;
  
  enum class GameType {
    None, Mario
  };

  class GameData {
  public:
    virtual ~GameData() = default;
  };
  
  /// This function is the defination of game data. Implementaiton should be at client
  std::unique_ptr<GameData> CreateGameData(GameType game_type);
  
}

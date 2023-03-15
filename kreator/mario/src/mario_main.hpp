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
    // Getters
    std::string GameName() const override { return "ikan_mario"; }
    std::string CbpRootDir() const override { return AM::ClientAsset("scenes/"); };
    std::vector<std::filesystem::path> FavDirecotries() const override {
      return {
        AM::ProjectPath("kreator/layers/ikan_game_editor/games/mario/assets"),
        AM::ClientAsset("scenes"),
      };
    }
  };
  
}

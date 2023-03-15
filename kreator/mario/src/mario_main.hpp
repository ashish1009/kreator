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
    std::string SavedScenePath() const override { return AM::ClientAsset("scenes/Mario_Scene.ikanScene"); };
    std::vector<std::filesystem::path> FavDirecotries() const override {
      return {
        AM::ProjectPath("kreator/mario/assets"),
        AM::ClientAsset("scenes"),
      };
    }
    ImguiFont RegularFontData() const override {
      return { AM::ClientAsset(font_path_), 14.0f };
    }
    ImguiFont BoldFontData() const override {
      return { AM::ClientAsset(font_path_), 14.0f };
    }

  private:
    std::string font_path_ = "fonts/mario.ttf";
  };
  
}

//
//  mario.hpp
//  kreator
//
//  Created by Ashish . on 08/02/23.
//

#pragma once

#include "renderer_layer.hpp"

namespace mario {
  
  using namespace ikan;
    
  class MarioData : public ikan_game::GameData {
  public:
    std::string GameName() override {
      return "Mario";
    }
    
    ImguiFont RegularFontData() override {
      return {
        AM::ClientAsset("fonts/mario.ttf"),
        14.0f
        /* Size of font */
      };
    }
    
    ImguiFont BoldFontData() override {
      return {
        AM::ClientAsset("fonts/mario.ttf"),
        14.0f /* Size of font */
      };
    }
    
    std::vector<std::filesystem::path> FavDirecotries() override {
      return {
        AM::ProjectPath("kreator/layers/mario/assets"),
        AM::ProjectPath("kreator/layers/mario/assets/scenes"),
      };
    }
  };
  
}

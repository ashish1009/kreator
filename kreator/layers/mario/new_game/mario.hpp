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
    void Init() override;
    void Update(Timestep ts) override;
    void RenderGui() override;
    
    // Getters
    std::string GameName() override;
    ImguiFont RegularFontData() override;
    ImguiFont BoldFontData() override;
    std::vector<std::filesystem::path> FavDirecotries() override;
  };
  
}

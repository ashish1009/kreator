//
//  sprite_manager.hpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

#include "enums.h"

namespace mario {
  
  using namespace ikan;
  
  class SpriteManager {
  public:
    static void Init();
    static void Shutdown();
    
    static std::shared_ptr<Texture> GetSpriteImage(SpriteType type);
    static std::vector<std::shared_ptr<SubTexture>> GetPlayerStateSprite(PlayerState type);

    MAKE_PURE_STATIC(SpriteManager);
    
  private:
    static void FillPlayerStateSprite();
  };
  
}

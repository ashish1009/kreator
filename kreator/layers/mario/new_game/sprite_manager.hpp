//
//  sprite_manager.hpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  enum class SpriteType {
    Player
  };

  class SpriteManager {
  public:
    static void Init();
    static void Shutdown();
    
    static std::shared_ptr<Texture> GetSpriteImage(SpriteType type);
    
    MAKE_PURE_STATIC(SpriteManager);
  };
  
}

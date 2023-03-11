//
//  sprite_manager.hpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  enum class PlayerAction;
  enum class PlayerState;
  
  enum class SpriteType {
    Player, Items, Enemy, Tile, StartScreen
  };
  
  class SpriteManager {
  public:
    static void Init();
    static void Shutdown();
    
    static std::shared_ptr<Texture> GetSpriteImage(SpriteType type);
    static const std::vector<std::shared_ptr<SubTexture>>& GetPlayerStateSprite(PlayerState state, PlayerAction action);
    
    MAKE_PURE_STATIC(SpriteManager);
  };
  
}

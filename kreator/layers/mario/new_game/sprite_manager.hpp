//
//  sprite_manager.hpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  enum class PlayerAction;
  enum class PlayerState;
  enum class EnemyState;
  
  enum class SpriteType {
    Player, Items, Enemy
  };
  
  class SpriteManager {
  public:
    static void Init();
    static void Shutdown();
    
    static std::shared_ptr<Texture> GetSpriteImage(SpriteType type);
    static std::vector<std::shared_ptr<SubTexture>> GetPlayerStateSprite(PlayerState state, PlayerAction action);
    static std::shared_ptr<SubTexture> GetGoombaSprite(EnemyState state);
    
    MAKE_PURE_STATIC(SpriteManager);
    
  private:
    static void FillStateSprite();
  };
  
}

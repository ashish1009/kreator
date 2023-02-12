//
//  sprite_manager.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "sprite_manager.hpp"

namespace mario {
    
  struct SpriteData {
    std::shared_ptr<Texture> player_sprite;
    
    std::unordered_map<SpriteType, std::shared_ptr<Texture>> sprite_map;
  };
  static SpriteData* data_;
  
  void SpriteManager::Init() {
    data_ = new SpriteData();
    
    data_->player_sprite = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), false);
    data_->sprite_map[SpriteType::Player] = data_->player_sprite;
  }
  
  void SpriteManager::Shutdown() {
    delete data_;
  }
  
  std::shared_ptr<Texture> SpriteManager::GetSpriteImage(SpriteType type) {
    if (!data_)
      return nullptr;
    
    if (data_->sprite_map.find(type) == data_->sprite_map.end())
      return nullptr;
    
    return data_->sprite_map.at(type);
  }
  
}

//
//  SpriteManager.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "SpriteManager.hpp"

namespace mario {

  struct SpriteData {
    std::unordered_map<SpriteType, std::shared_ptr<Texture>> sprite_map;
  };
  static SpriteData* data_;

  void SpriteManager::Init() {
    data_ = new SpriteData();
    
    // Store Textures
    data_->sprite_map[SpriteType::Player] = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), false);
    data_->sprite_map[SpriteType::Items] = Renderer::GetTexture(AM::ClientAsset("textures/item.png"), false);
    data_->sprite_map[SpriteType::Enemy] = Renderer::GetTexture(AM::ClientAsset("textures/enemy.png"), false);
    data_->sprite_map[SpriteType::Tile] = Renderer::GetTexture(AM::ClientAsset("textures/tile.png"), false);
    data_->sprite_map[SpriteType::StartScreen] = Renderer::GetTexture(AM::ClientAsset("textures/start_screen.png"), false);    
  }
  
  std::shared_ptr<Texture> SpriteManager::GetSpriteImage(SpriteType type) {
    if (data_ and data_->sprite_map.find(type) != data_->sprite_map.end())
      return data_->sprite_map.at(type);
    return nullptr;
  }

  void SpriteManager::Shutdown() {
    delete data_;
  }

  
}

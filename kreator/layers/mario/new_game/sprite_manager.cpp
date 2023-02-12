//
//  sprite_manager.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "sprite_manager.hpp"
#include "player.hpp"

namespace mario {
    
  struct SpriteData {
    std::shared_ptr<Texture> player_sprite;
    std::unordered_map<SpriteType, std::shared_ptr<Texture>> sprite_map;
    std::unordered_map<PlayerState, std::vector<std::shared_ptr<SubTexture>>> player_subtextures_map;
  };
  static SpriteData* data_;
  
  void SpriteManager::Init() {
    data_ = new SpriteData();
    
    data_->player_sprite = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), false);
    data_->sprite_map[SpriteType::Player] = data_->player_sprite;
    
    FillPlayerStateSprite();
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
  
  std::vector<std::shared_ptr<SubTexture>> SpriteManager::GetPlayerStateSprite(PlayerState state) {
    if (!data_)
      return {nullptr};
    
    if (data_->player_subtextures_map.find(state) == data_->player_subtextures_map.end())
      return {nullptr};

    return data_->player_subtextures_map.at(state);
  }

  
  void SpriteManager::FillPlayerStateSprite() {
    // Small Player
    data_->player_subtextures_map[PlayerState::Idle].push_back(SubTexture::CreateFromCoords(data_->player_sprite, {6.0f, 30.0f}));
    
    data_->player_subtextures_map[PlayerState::Run].push_back(SubTexture::CreateFromCoords(data_->player_sprite, {0.0f, 30.0f}));
    data_->player_subtextures_map[PlayerState::Run].push_back(SubTexture::CreateFromCoords(data_->player_sprite, {1.0f, 30.0f}));
    data_->player_subtextures_map[PlayerState::Run].push_back(SubTexture::CreateFromCoords(data_->player_sprite, {2.0f, 30.0f}));
  }
  
}

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
    std::unordered_map<SpriteType, std::shared_ptr<Texture>> sprite_map;
    
    // Player
    std::unordered_map<PlayerState, std::vector<std::shared_ptr<SubTexture>>> player_subtextures_map;
  };
  static SpriteData* data_;
  
  void SpriteManager::Init() {
    data_ = new SpriteData();
    
    data_->sprite_map[SpriteType::Player] = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), false);
    data_->sprite_map[SpriteType::Items] = Renderer::GetTexture(AM::ClientAsset("textures/items.png"), false);
    
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
    const auto& player_sprite = GetSpriteImage(SpriteType::Player);
    // Small Player
    data_->player_subtextures_map[PlayerState::Idle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 30.0f}));
    data_->player_subtextures_map[PlayerState::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 30.0f}));
    data_->player_subtextures_map[PlayerState::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 30.0f}));
    data_->player_subtextures_map[PlayerState::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 30.0f}));
    data_->player_subtextures_map[PlayerState::SwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 30.0f}));
    data_->player_subtextures_map[PlayerState::Jump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 30.0f}));

    // Big Player
    data_->player_subtextures_map[PlayerState::BigIdle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 31.0f}));
    data_->player_subtextures_map[PlayerState::BigRun].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 31.0f}));
    data_->player_subtextures_map[PlayerState::BigRun].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 31.0f}));
    data_->player_subtextures_map[PlayerState::BigRun].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 31.0f}));
    data_->player_subtextures_map[PlayerState::BigSwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 31.0f}));
    data_->player_subtextures_map[PlayerState::BigJump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 31.0f}));
  }
  
}

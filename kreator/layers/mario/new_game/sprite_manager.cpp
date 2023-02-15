//
//  sprite_manager.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "sprite_manager.hpp"
#include "player.hpp"
#include "enemy.hpp"

namespace mario {
    
  struct SpriteData {
    std::unordered_map<SpriteType, std::shared_ptr<Texture>> sprite_map;
    
    // Player
    std::unordered_map<PlayerState, std::unordered_map<PlayerAction, std::vector<std::shared_ptr<SubTexture>>>> player_subtextures_map;
    std::unordered_map<EnemyState, std::shared_ptr<SubTexture>> enemy_subtextures_map;
  };
  static SpriteData* data_;
  
  void SpriteManager::Init() {
    data_ = new SpriteData();
    
    data_->sprite_map[SpriteType::Player] = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), false);
    data_->sprite_map[SpriteType::Items] = Renderer::GetTexture(AM::ClientAsset("textures/items.png"), false);
    data_->sprite_map[SpriteType::Enemy] = Renderer::GetTexture(AM::ClientAsset("textures/enemies.png"), false);

    FillStateSprite();
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
  
  std::vector<std::shared_ptr<SubTexture>> SpriteManager::GetPlayerStateSprite(PlayerState state, PlayerAction action) {
    if (!data_)
      return {nullptr};
    
    if (data_->player_subtextures_map.find(state) == data_->player_subtextures_map.end())
      return {nullptr};
    
    const auto& player_map = data_->player_subtextures_map.at(state);
    
    if (player_map.find(action) == player_map.end())
      return {nullptr};

    return player_map.at(action);
  }

  std::shared_ptr<SubTexture> SpriteManager::GetGoombaSprite(EnemyState state) {
    if (!data_)
      return nullptr;

    if (data_->enemy_subtextures_map.find(state) == data_->enemy_subtextures_map.end())
      return nullptr;
    
    return data_->enemy_subtextures_map.at(state);
  }

  void SpriteManager::FillStateSprite() {
    const auto& player_sprite = GetSpriteImage(SpriteType::Player);
    
    // Small Player
    auto& small_player_map = data_->player_subtextures_map[PlayerState::Small];
    small_player_map[PlayerAction::Idle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 30.0f}));
    small_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 30.0f}));
    small_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 30.0f}));
    small_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 30.0f}));
    small_player_map[PlayerAction::SwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 30.0f}));
    small_player_map[PlayerAction::Jump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 30.0f}));

    // Big Player
    auto& Big_player_map = data_->player_subtextures_map[PlayerState::Big];
    Big_player_map[PlayerAction::Idle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 31.0f}, {1.0f, 2.0f}));
    Big_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 31.0f}, {1.0f, 2.0f}));
    Big_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 31.0f}, {1.0f, 2.0f}));
    Big_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 31.0f}, {1.0f, 2.0f}));
    Big_player_map[PlayerAction::SwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 31.0f}, {1.0f, 2.0f}));
    Big_player_map[PlayerAction::Jump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 31.0f}, {1.0f, 2.0f}));

    // Fire Player
    auto& fire_player_map = data_->player_subtextures_map[PlayerState::Fire];
    fire_player_map[PlayerAction::Idle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::SwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Jump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 28.0f}, {1.0f, 2.0f}));

    const auto& enemy_sprite = GetSpriteImage(SpriteType::Enemy);
    data_->enemy_subtextures_map[EnemyState::Dead] = SubTexture::CreateFromCoords(enemy_sprite, {2.0f, 6.0f});
  }
  
}

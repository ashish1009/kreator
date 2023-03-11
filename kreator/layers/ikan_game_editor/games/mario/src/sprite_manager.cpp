//
//  sprite_manager.cpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#include "sprite_manager.hpp"
#include "player.hpp"
#include "enemy.hpp"

namespace mario {
  
  struct SpriteData {
    std::unordered_map<SpriteType, std::shared_ptr<Texture>> sprite_map;
    std::unordered_map<PlayerState, std::unordered_map<PlayerAction, std::vector<std::shared_ptr<SubTexture>>>> player_subtextures_map;
    std::unordered_map<EnemyType, std::unordered_map<EnemyState, std::shared_ptr<SubTexture>>> enemy_subtextures_map;
  };
  static SpriteData* data_;
  
  void SpriteManager::Init() {
    data_ = new SpriteData();
    
    // ---------------
    // Store Textures
    // ---------------
    data_->sprite_map[SpriteType::Player] = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), false);
    data_->sprite_map[SpriteType::Items] = Renderer::GetTexture(AM::ClientAsset("textures/item.png"), false);
    data_->sprite_map[SpriteType::Enemy] = Renderer::GetTexture(AM::ClientAsset("textures/enemy.png"), false);
    data_->sprite_map[SpriteType::Tile] = Renderer::GetTexture(AM::ClientAsset("textures/tile.png"), false);
    data_->sprite_map[SpriteType::StartScreen] = Renderer::GetTexture(AM::ClientAsset("textures/start_screen.png"), false);
    
    // ---------------
    // Player
    // ---------------
    const auto& player_sprite = GetSpriteImage(SpriteType::Player);
    
    // Small Player
    auto& small_player_map = data_->player_subtextures_map[PlayerState::Small];
    small_player_map[PlayerAction::Idle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 30.0f}));
    small_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 30.0f}));
    small_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 30.0f}));
    small_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 30.0f}));
    small_player_map[PlayerAction::SwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 30.0f}));
    small_player_map[PlayerAction::Jump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 30.0f}));
    small_player_map[PlayerAction::Die].push_back(SubTexture::CreateFromCoords(player_sprite, {5.0f, 30.0f}));
    
    // Big Player
    auto& big_player_map = data_->player_subtextures_map[PlayerState::Big];
    big_player_map[PlayerAction::Idle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 31.0f}, {1.0f, 2.0f}));
    big_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 31.0f}, {1.0f, 2.0f}));
    big_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 31.0f}, {1.0f, 2.0f}));
    big_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 31.0f}, {1.0f, 2.0f}));
    big_player_map[PlayerAction::SwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 31.0f}, {1.0f, 2.0f}));
    big_player_map[PlayerAction::Jump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 31.0f}, {1.0f, 2.0f}));
        
    // Fire Player
    auto& fire_player_map = data_->player_subtextures_map[PlayerState::Fire];
    fire_player_map[PlayerAction::Idle].push_back(SubTexture::CreateFromCoords(player_sprite, {6.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {0.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {1.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Run].push_back(SubTexture::CreateFromCoords(player_sprite, {2.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::SwitchSide].push_back(SubTexture::CreateFromCoords(player_sprite, {3.0f, 28.0f}, {1.0f, 2.0f}));
    fire_player_map[PlayerAction::Jump].push_back(SubTexture::CreateFromCoords(player_sprite, {4.0f, 28.0f}, {1.0f, 2.0f}));
    
    const auto& enemy_sprite = GetSpriteImage(SpriteType::Enemy);
    auto& goomba_sprite = data_->enemy_subtextures_map[EnemyType::Goomba];
    goomba_sprite[EnemyState::Dead] = SubTexture::CreateFromCoords(enemy_sprite, {2.0f, 6.0f});
    
    auto& duck_sprite = data_->enemy_subtextures_map[EnemyType::Duck];
    duck_sprite[EnemyState::Dying] = SubTexture::CreateFromCoords(enemy_sprite, {10.0f, 6.0f});
  }
  
  std::shared_ptr<Texture> SpriteManager::GetSpriteImage(SpriteType type) {
    if (data_ and data_->sprite_map.find(type) != data_->sprite_map.end())
      return data_->sprite_map.at(type);
    return nullptr;
  }
  
  const std::vector<std::shared_ptr<SubTexture>>& SpriteManager::GetPlayerStateSprite(PlayerState state, PlayerAction action) {
    IK_ASSERT(data_ or data_->player_subtextures_map.find(state) != data_->player_subtextures_map.end());
    
    const auto& player_map = data_->player_subtextures_map.at(state);
    IK_ASSERT(player_map.find(action) != player_map.end());

    return player_map.at(action);
  }
  
  std::shared_ptr<SubTexture> SpriteManager::GetEnemySprite(EnemyType type, EnemyState state) {
    if (data_ and data_->enemy_subtextures_map.find(type) != data_->enemy_subtextures_map.end()) {
      const auto& goomba_map = data_->enemy_subtextures_map.at(type);
      if (goomba_map.find(state) != goomba_map.end()) {
        return goomba_map.at(state);
      }
    }
    return nullptr;
  }
  
  void SpriteManager::Shutdown() {
    delete data_;
  }
  
  
}

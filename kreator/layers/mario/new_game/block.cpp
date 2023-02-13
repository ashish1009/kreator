//
//  block_script.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "block.hpp"
#include "runtime_items.hpp"

namespace mario {
  
  BlockController::BlockController(Type type, uint32_t count) : type_(type), count_(count) {
    switch (type_) {
      case Type::Empty : {
        break;
      }
      case Type::Coin : {
        break;
      }
      case Type::Star : {
        break;
      }
      case Type::PowerUp : {
        break;
      }
      default:
        break;
    }
  }
    
  void BlockController::Create(Entity entity) {
    entity_ = entity;
    start_pos_ = glm::vec2(entity_.GetComponent<TransformComponent>().Translation());
    end_pos_ = start_pos_ + glm::vec2(0.0f, 0.2f);
  }
  
  void BlockController::Update(Timestep ts) {
    auto& tc = entity_.GetComponent<TransformComponent>();
    if (animation_) {
      if (going_up_) {
        if (tc.Translation().y < end_pos_.y) {
          tc.UpdateTranslation_Y(tc.Translation().y + (ts * speed_));
        }
        else {
          going_up_ = false;
        }
      }
      else {
        if (tc.Translation().y > start_pos_.y) {
          tc.UpdateTranslation_Y(tc.Translation().y - (ts * speed_));
        }
        else {
          tc.UpdateTranslation_Y(start_pos_.y);
          going_up_ = true;
          animation_ = false;
        }
      }
    }
  }
  
  void BlockController::BeginCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 contact_normal) {
    PlayerController* pc = PlayerController::Get();
    if (active_ and pc and contact_normal.y < -0.8f) {
      animation_ = true;
      PlayerHit(pc);
    }
  }
  
  void BlockController::PlayerHit(PlayerController* pc) {
    auto& tc = entity_.GetComponent<TransformComponent>();
    switch (type_) {
      case Type::Empty : {
        if (!pc->IsSmall()) {
          // Play sound // Break
          // Destory the object
        }
        break;
      }
      case Type::Coin: {
        RuntimeItem::Create(Items::Coin, entity_.GetScene(), {tc.Translation().x, tc.Translation().y + 1});
        count_--;
        if (count_ == 0)
          SetInactive();

        break;
      }
      case Type::Star: {
        break;
      }
      case Type::PowerUp : {
        RuntimeItem::Create(Items::Mushroom, entity_.GetScene(), {tc.Translation().x, tc.Translation().y + 1.2});
        SetInactive();
        break;
      }
      default:
        break;
    }
  }
  
  struct BlockData {
    BlockController::Type type;
    ScriptLoaderFn loader_fun;
    uint32_t coin_count;
    
    BlockData() = default;
    BlockData(BlockController::Type type, ScriptLoaderFn loader_fun, uint32_t coin_count)
    : type(type), loader_fun(loader_fun), coin_count(coin_count) {}
  };

  struct BlockScriptManagerData {
    std::unordered_map<std::string, BlockData> block_map;
  };
  static BlockScriptManagerData* data;
  
  void BlockScriptManager::Init() {
    data = new BlockScriptManagerData();
    
    auto brick_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockController::Type::Empty, 0);
        return true;
      }
      return false;
    };
    
    auto coin_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockController::Type::Coin, 1);
        return true;
      }
      return false;
    };
    
    auto multi_coin_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockController::Type::Coin, 10);
        return true;
      }
      return false;
    };

    auto powerup_coin_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockController::Type::PowerUp, 0);
        return true;
      }
      return false;
    };

    data->block_map["Brick"] = {BlockController::Type::Empty, brick_loader_fn, 0};
    data->block_map["Coin"] = {BlockController::Type::Coin, coin_loader_fn, 1};
    data->block_map["MultiCoin"] = {BlockController::Type::Coin, multi_coin_loader_fn, 10};
    data->block_map["PowerUp"] = {BlockController::Type::PowerUp, powerup_coin_loader_fn, 0};
  }

  void BlockScriptManager::Shutdown() {
    delete data;
  }

  ScriptLoaderFn BlockScriptManager::GetLoaderFn(const std::string& tag) {
    return data->block_map.at(tag).loader_fun;
  }

  BlockController::Type BlockScriptManager::GetType(const std::string& tag) {
    return data->block_map.at(tag).type;
  }

  uint32_t BlockScriptManager::GetCount(const std::string& tag) {
    return data->block_map.at(tag).coin_count;
  }

}

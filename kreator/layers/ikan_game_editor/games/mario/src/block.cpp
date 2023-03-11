//
//  block.cpp
//  kreator
//
//  Created by Ashish . on 11/03/23.
//

#include "block.hpp"

namespace mario {
  
  BlockController::BlockController(BlockType type, uint32_t count)
  : type_(type), count_(count) {
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
          tc.AddTranslation_Y(ts * speed_);
        }
        else {
          going_up_ = false;
        }
      }
      else {
        if (tc.Translation().y > start_pos_.y) {
          tc.AddTranslation_Y(-(ts * speed_));
        }
        else {
          tc.UpdateTranslation_Y(start_pos_.y);
          going_up_ = true;
          animation_ = false;
        }
      }
    }
  }
  
  void BlockController::BeginCollision(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    PlayerController* pc = PlayerController::Get();
    if (active_ and pc and contact_normal.y < -0.8f) {
      animation_ = true;
      PlayerHit(pc);
    }
  }

  void BlockController::PlayerHit(PlayerController* pc) {
    switch (type_) {
      case BlockType::Empty : {
        break;
      }
      case BlockType::Coin: {
        break;
      }
      case BlockType::Star: {
        break;
      }
      case BlockType::PowerUp : {
        break;
      }
      default:
        break;
    }
  }
  
  void BlockController::RenderGui() {
    ImGui::Text("Active | %s", active_ ? "True" : "False");
  }
  
  void BlockController::Copy(void* script) {
    if (!script)
      return;
    
    BlockController* block_script = reinterpret_cast<BlockController*>(script);
    IK_ASSERT(block_script);
    
    going_up_ = block_script->going_up_;
    animation_ = block_script->animation_;
    start_pos_ = block_script->start_pos_;
    end_pos_ = block_script->end_pos_;
    
    active_ = block_script->active_;
    speed_ = block_script->speed_;
    
    type_ = block_script->type_;
    count_ = block_script->count_;
  }
  
  struct BlockData {
    BlockType type;
    ScriptLoaderFn loader_fun;
    uint32_t coin_count;
    
    BlockData() = default;
    BlockData(BlockType type, ScriptLoaderFn loader_fun, uint32_t coin_count)
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
        sc->Bind<mario::BlockController>(mario::BlockType::Empty, 0);
        return true;
      }
      return false;
    };
    
    auto coin_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockType::Coin, 1);
        return true;
      }
      return false;
    };
    
    auto multi_coin_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockType::Coin, 10);
        return true;
      }
      return false;
    };
    
    auto powerup_coin_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockType::PowerUp, 0);
        return true;
      }
      return false;
    };
    
    data->block_map["Brick"] = {BlockType::Empty, brick_loader_fn, 0};
    data->block_map["CoinBlock"] = {BlockType::Coin, coin_loader_fn, 1};
    data->block_map["MultiCoinBlock"] = {BlockType::Coin, multi_coin_loader_fn, 10};
    data->block_map["PowerUpBlock"] = {BlockType::PowerUp, powerup_coin_loader_fn, 0};
  }
  
  void BlockScriptManager::Shutdown() {
    delete data;
  }
  
  ScriptLoaderFn BlockScriptManager::GetLoaderFn(const std::string& tag) {
    return data->block_map.at(tag).loader_fun;
  }
  
  BlockType BlockScriptManager::GetType(const std::string& tag) {
    return data->block_map.at(tag).type;
  }
  
  uint32_t BlockScriptManager::GetCount(const std::string& tag) {
    return data->block_map.at(tag).coin_count;
  }

  
}

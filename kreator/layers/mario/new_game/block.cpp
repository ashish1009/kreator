//
//  block_script.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "block.hpp"
#include "coin.hpp"

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
    switch (type_) {
      case Type::Empty : {
        if (!pc->IsSmall()) {
          // Play sound // Break
          // Destory the object
        }
        break;
      }
      case Type::Coin : {
        auto& tc = entity_.GetComponent<TransformComponent>();
        Coin::CreateBlockCoin(entity_.GetScene(), {tc.Translation().x, tc.Translation().y + 1});
        count_--;
        
        if (count_ == 0) {
          auto& qc = entity_.GetComponent<QuadComponent>();
          const auto& tex = qc.texture_comp.component;
          qc.texture_comp.sprite = SubTexture::CreateFromCoords(tex, {27.0f, 27.0f});
          
          if (entity_.HasComponent<AnimationComponent>())
            entity_.RemoveComponent<AnimationComponent>();
          
          active_ = false; 
        }
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

  struct BlockScriptManagerData {
    std::unordered_map<std::string, ScriptLoaderFn> loader_map;
    std::unordered_map<std::string, BlockController::Type> type_map;
    std::unordered_map<std::string, uint32_t> count_map;
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


    data->loader_map["Brick"] = brick_loader_fn;
    data->loader_map["Coin"] = coin_loader_fn;
    data->loader_map["MultiCoin"] = multi_coin_loader_fn;

    data->type_map["Brick"] = BlockController::Type::Empty;
    data->type_map["Coin"] = BlockController::Type::Coin;
    data->type_map["MultiCoin"] = BlockController::Type::Coin;
    
    data->count_map["Brick"] = 0;
    data->count_map["Coin"] = 1;
    data->count_map["MultiCoin"] = 10;
  }

  void BlockScriptManager::Shutdown() {
    delete data;
  }

  ScriptLoaderFn BlockScriptManager::GetLoaderFn(const std::string& tag) {
    return data->loader_map.at(tag);
  }

  BlockController::Type BlockScriptManager::GetType(const std::string& tag) {
    return data->type_map.at(tag);
  }

  uint32_t BlockScriptManager::GetCount(const std::string& tag) {
    return data->count_map.at(tag);
  }

}

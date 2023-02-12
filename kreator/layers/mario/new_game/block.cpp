//
//  block_script.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "block.hpp"

namespace mario {
  
  BlockController::BlockController(Type type) : type_(type) {
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
    end_pos_ = start_pos_ + glm::vec2(0.0f, 0.4f);
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
      switch (type_) {
        case Type::Empty : {
          animation_ = true;
          PlayerHit(pc);
          // ADD Suund
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
  }
  
  void BlockController:: PlayerHit(PlayerController* pc) {
    if (!pc->IsSmall()) {
      // Play sound // Break
      // Destory the object
    }
  }

  struct BlockScriptManagerData {
    std::unordered_map<std::string, ScriptLoaderFn> loader_map;
    std::unordered_map<std::string, BlockController::Type> type_map;
  };
  static BlockScriptManagerData* data;
  
  void BlockScriptManager::Init() {
    data = new BlockScriptManagerData();
    
    auto brick_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockController::Type::Empty);
        return true;
      }
      return false;
    };
    
    auto coin_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::BlockController") {
        sc->Bind<mario::BlockController>(mario::BlockController::Type::Coin);
        return true;
      }
      return false;
    };

    data->loader_map["Brick"] = brick_loader_fn;
    data->loader_map["Coin"] = coin_loader_fn;
    
    data->type_map["Brick"] = BlockController::Type::Empty;
    data->type_map["Coin"] = BlockController::Type::Coin;
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


}

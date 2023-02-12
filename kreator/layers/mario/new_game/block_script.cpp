//
//  block_script.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "block_script.hpp"

namespace mario {
  
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

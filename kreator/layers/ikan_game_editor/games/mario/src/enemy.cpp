//
//  enemy.cpp
//  kreator
//
//  Created by Ashish . on 11/03/23.
//

#include "enemy.hpp"
#include "player.hpp"
#include "sprite_manager.hpp"

namespace mario {
 
  struct EnemyScriptData {
    std::unordered_map<EnemyType, EnemyData> script_map;
  };
  static EnemyScriptData* data;
  
  const EnemyData& EnemyScriptManager::GetData(EnemyType type) {
    return data->script_map.at(type);
  }
  void EnemyScriptManager::Init() {
    data = new EnemyScriptData();
    
    static auto enemy_load_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::EnemyController") {
        sc->Bind<mario::EnemyController>(EnemyType::Goomba);
        return true;
      }
      return false;
    };
    
    data->script_map[EnemyType::Goomba] = {EnemyType::Goomba, enemy_load_fn};
    data->script_map[EnemyType::Duck] = {EnemyType::Duck, enemy_load_fn};
  }
  void EnemyScriptManager::Shutdown() {
    delete data;
  }

}

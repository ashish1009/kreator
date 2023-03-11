//
//  enemy.hpp
//  kreator
//
//  Created by Ashish . on 11/03/23.
//

#pragma once

#include "runtime_item.hpp"

namespace mario {
  
  using namespace ikan;
  
  enum class EnemyType {
    Goomba, Duck
  };
  
  enum class EnemyState {
    Dying, Dead
  };

  class EnemyController : public ScriptableEntity {
  public:
    EnemyController(EnemyType type) { type_ = type; }

  private:
    EnemyType type_;
  };
  
  struct EnemyData {
    EnemyType type;
    ScriptLoaderFn loader_fun;
    EnemyData() = default;
    EnemyData(EnemyType type, ScriptLoaderFn fun) : type(type), loader_fun(fun) { }
  };
  
  class EnemyScriptManager {
  public:
    static void Init();
    static void Shutdown();
    
    static const EnemyData& GetData(EnemyType type);
  };
  using ESM = EnemyScriptManager;
  
  
  inline bool IsEnemy(const std::string& tag) {
    return tag == "Goomba" or tag == "Duck";
  }
  
  inline EnemyType GetType(const std::string& tag) {
    if (tag == "Goomba") return EnemyType::Goomba;
    if (tag == "Duck") return EnemyType::Duck;
    return EnemyType::Goomba;
  }

}

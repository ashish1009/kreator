//
//  scriptable_entity.h
//  ikan
//
//  Created by Ashish . on 07/01/23.
//


#pragma once

#include "scene/entity.hpp"

namespace ikan {
  
  // ---------------------------------------------------------------------------
  // This file holds all the native scripts. For now we are using only C type
  // Scripts so we have to pre define all of them. No Runtime script is supported
  // yet. So store all the script name as their class name in ScriptManager::scripts_
  // ---------------------------------------------------------------------------
  
  class NativeScriptComponent;
  
  class ScriptManager {
  public:
    static std::vector<std::string> scripts_;

    /// This function update the scrip bind function pointer
    /// - Parameters:
    ///   - sc: Native script component pointer
    ///   - script_name: Script name
    static void UpdateScript(NativeScriptComponent* sc, const std::string& script_name);
    
    MAKE_PURE_STATIC(ScriptManager);
  };
  
  class ScriptableEntity {
  public:
    virtual ~ScriptableEntity() {}

    template<typename T>
    T& GetComponent() {
      return entity_.GetComponent<T>();
    }

  protected:
    virtual void Create() {}
    virtual void Destroy() {}
    virtual void Update(Timestep ts) {}

  protected:
    Entity entity_;
    EnttScene* scene_;
    friend class EnttScene;
  };
  
}

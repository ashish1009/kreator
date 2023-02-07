//
//  scriptable_entity.cpp
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#include "scriptable_entity.hpp"
#include "ecs/core_components.hpp"
#include "core/input.hpp"

namespace ikan {
  
  // --------------------------------------------------------------------------
  // Script Manager
  // --------------------------------------------------------------------------
  std::vector<std::string> ScriptManager::scripts_ = {
    "Select Script"
  };

  void ScriptManager::UpdateScript(NativeScriptComponent* sc,
                                   const std::string& script_name,
                                   ScriptLoaderFn loader_function) {
    if (script_name == "ikan::ScriptableEntity")
      sc->Bind<ikan::ScriptableEntity>();
    else {
      IK_ASSERT(loader_function, "Invalid Script name");
      bool script_loaded = loader_function(sc, script_name);
      IK_ASSERT(script_loaded, "Invalid Script name");
    }
  }
  
}

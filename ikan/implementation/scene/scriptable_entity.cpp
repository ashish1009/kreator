//
//  scriptable_entity.cpp
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#include "scriptable_entity.hpp"
#include "scene/core_components.hpp"
#include "core/input.hpp"

namespace ikan {
  
  // --------------------------------------------------------------------------
  // Script Manager
  // --------------------------------------------------------------------------
  std::vector<std::string> ScriptManager::scripts_ = {
    "ikan::ScriptableEntity",
  };
  
  void ScriptManager:: UpdateScript(NativeScriptComponent* sc,
                                    const std::string& script_name) {
    if (script_name == "editor::ScriptableEntity")
      sc->Bind<ikan::ScriptableEntity>();
    else
      IK_ASSERT(false, "Invalid Script name");
  }

  // --------------------------------------------------------------------------
  // Freefall controller class
  // --------------------------------------------------------------------------
  void FreeFallController::Update(Timestep ts) {
  }
  
}

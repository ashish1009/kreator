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
    "Select Script",
    "ikan::CameraController"
  };

  void ScriptManager::UpdateScript(NativeScriptComponent* sc, const std::string& script_name, ScriptLoaderFn loader_function) {
    if (script_name == "ikan::CameraController") {
      sc->Bind<ikan::CameraController>();
    }
    else {
      if (loader_function) {
        bool script_loaded = loader_function(sc, script_name);
        IK_ASSERT(script_loaded, "Invalid Script name");
      }
      else {
        sc->Bind<ikan::ScriptableEntity>();
      }
    }
  }

  // --------------------------------------------------------------------------
  // Camera Controller
  // --------------------------------------------------------------------------
  void CameraController::Update(Timestep ts) {
//    auto& tc = GetComponent<TransformComponent>();
//
//    if (Input::IsKeyPressed(KeyCode::Left))
//      tc.UpdateTranslation_X(tc.Translation().x - (speed_ * ts));
//    if (Input::IsKeyPressed(KeyCode::Right))
//      tc.UpdateTranslation_X(tc.Translation().x + (speed_ * ts));
  }
  void CameraController::RenderGui() {
    PropertyGrid::Float1("Speed", speed_);
  }
  
}

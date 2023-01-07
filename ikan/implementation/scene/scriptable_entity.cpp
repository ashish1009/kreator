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
  void ScriptManager::UpdateScript(NativeScriptComponent* sc,
                                   const std::string& script_name,
                                   ScriptLoaderFn loader_function) {
    if (script_name == "ikan::ScriptableEntity")
      sc->Bind<ikan::ScriptableEntity>();
    if (script_name == "ikan::FreeFallController")
      sc->Bind<ikan::FreeFallController>();
    else {
      bool script_loaded = loader_function(sc, script_name);
      IK_ASSERT(script_loaded, "Invalid Script name");
    }
  }
  
  // --------------------------------------------------------------------------
  // Scriptable Entity Base class
  // --------------------------------------------------------------------------
  bool ScriptableEntity::CollisionDetected(const AABB& aabb) {
    return false;
  }

  // --------------------------------------------------------------------------
  // Freefall controller class
  // --------------------------------------------------------------------------
  void FreeFallController::Update(Timestep ts) {
    // Dummy copy of entity y Position
    auto translation = GetComponent<TransformComponent>().translation;
    translation.y -= speed_ * ts;
    
    auto& tc = GetComponent<TransformComponent>();
    AABB aabb;
    
    // If no collision then update the position
    if (!CollisionDetected(aabb))
      tc.translation = translation;
  }
  
}

//
//  scriptable_entity.cpp
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#include "scriptable_entity.hpp"
#include "ecs/core_components.hpp"
#include "core/input.hpp"

namespace ecs {
  
  // --------------------------------------------------------------------------
  // Script Manager
  // --------------------------------------------------------------------------
  std::vector<std::string> ScriptManager::scripts_ = {
    "Select Script",
    "ecs::MovementController",
  };

  void ScriptManager::UpdateScript(NativeScriptComponent* sc,
                                   const std::string& script_name,
                                   ScriptLoaderFn loader_function) {
    if (script_name == "ecs::ScriptableEntity")
      sc->Bind<ecs::ScriptableEntity>();
    if (script_name == "ecs::MovementController")
      sc->Bind<ecs::MovementController>();
    else {
      IK_ASSERT(loader_function, "Invalid Script name");
      bool script_loaded = loader_function(sc, script_name);
      IK_ASSERT(script_loaded, "Invalid Script name");
    }
  }
  
  // --------------------------------------------------------------------------
  // Scriptable Entity Base class
  // --------------------------------------------------------------------------
  bool ScriptableEntity::CollisionDetected(const AABB& aabb) {
    bool collision_detected = false;
    // Collistion deteection
    scene_->GetRegistry().view<RigidBodyComponent>().each([&](entt::entity entity, auto& rc)
                                                          {
      // TODO: If no ther entity is there then no free fall.
      if (entity_ == entity)
        return;
      
      auto& other_tc = scene_->GetRegistry().get<TransformComponent>(entity);
      const AABB& original_aabb = scene_->GetRegistry().get<RigidBodyComponent>(entity).aabb;
      AABB world_aabb = original_aabb.GetWorldPosBoundingBox(other_tc.GetTransform());
      
      if (
          world_aabb.min.x <= aabb.max.x &&
          world_aabb.max.x >= aabb.min.x &&
          world_aabb.min.y <= aabb.max.y &&
          world_aabb.max.y >= aabb.min.y &&
          world_aabb.min.z <= aabb.max.z &&
          world_aabb.max.z >= aabb.min.z) {
            collision_detected = true;
            return;
          }
    });
    return collision_detected;
  }
  
  // ------------------------------------------------------------------------
  // Movement controller
  // ------------------------------------------------------------------------
  void MovementController::Update(Timestep ts) {
    // Dummy copy of entity Position
    auto translation = GetComponent<TransformComponent>().Translation();
    
    if (Input::IsKeyPressed(KeyCode::Left))
      translation.x -= speed_ * ts;
    if (Input::IsKeyPressed(KeyCode::Right))
      translation.x += speed_ * ts;
    
    auto& tc = GetComponent<TransformComponent>();
    const AABB& original_aabb = GetComponent<RigidBodyComponent>().aabb;
    AABB world_aabb = original_aabb.GetWorldPosBoundingBox(Math::GetTransformMatrix(translation,
                                                                                    tc.Rotation(),
                                                                                    tc.Scale()));
    
    // If no collision then update the position
    if (!CollisionDetected(world_aabb))
      tc.UpdateTranslation(translation);
  }
  
  void MovementController::RenderGui() {
    ImGui::Text("Speed : %f", speed_);
  }
  
}

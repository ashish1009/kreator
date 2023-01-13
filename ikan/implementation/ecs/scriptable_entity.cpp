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
      
      if (scene_->GetRegistry().has<QuadComponent>(entity)) {
        auto& other_tc = scene_->GetRegistry().get<TransformComponent>(entity);
        AABB world_aabb = AABB::GetWorldAABBPos(other_tc.GetTransform());
        
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
      } else if (scene_->GetRegistry().has<CircleComponent>(entity)) {
        auto& other_tc = scene_->GetRegistry().get<TransformComponent>(entity);
        BoundingCircle other_circle = BoundingCircle::GetWorldSpherePos(other_tc.Translation(), other_tc.Rotation(), other_tc.Scale());

        // get box closest point to sphere center by clamping
        const float x = glm::max(aabb.min.x, glm::min(other_circle.position.x, aabb.max.x));
        const float y = glm::max(aabb.min.y, glm::min(other_circle.position.y, aabb.max.y));
        const float z = glm::max(aabb.min.z, glm::min(other_circle.position.z, aabb.max.z));
        
        // this is the same as isPointInsideSphere
        const float distance = glm::sqrt(
                                         (x - other_circle.position.x) * (x - other_circle.position.x) +
                                         (y - other_circle.position.y) * (y - other_circle.position.y) +
                                         (z - other_circle.position.z) * (z - other_circle.position.z)
                                         );
        if (distance <= other_circle.radius.x and
            distance <= other_circle.radius.y and
            distance <= other_circle.radius.z) {
          collision_detected = true;
          return;
        }

      }
      
    });
    return collision_detected;
  }

  bool ScriptableEntity::CollisionDetected(const BoundingCircle& circle) {
    bool collision_detected = false;
    // Collistion deteection
    scene_->GetRegistry().view<RigidBodyComponent>().each([&](entt::entity entity, auto& rc)
                                                          {
      // TODO: If no ther entity is there then no free fall.
      if (entity_ == entity)
        return;
      
      if (scene_->GetRegistry().has<QuadComponent>(entity)) {
        auto& other_tc = scene_->GetRegistry().get<TransformComponent>(entity);
        AABB world_aabb = AABB::GetWorldAABBPos(other_tc.GetTransform());
        
        // get box closest point to sphere center by clamping
        const float x = glm::max(world_aabb.min.x, glm::min(circle.position.x, world_aabb.max.x));
        const float y = glm::max(world_aabb.min.y, glm::min(circle.position.y, world_aabb.max.y));
        const float z = glm::max(world_aabb.min.z, glm::min(circle.position.z, world_aabb.max.z));
        
        // this is the same as isPointInsideSphere
        const float distance = glm::sqrt(
                                   (x - circle.position.x) * (x - circle.position.x) +
                                   (y - circle.position.y) * (y - circle.position.y) +
                                   (z - circle.position.z) * (z - circle.position.z)
                                   );
        if (distance <= circle.radius.x and
            distance <= circle.radius.y and
            distance <= circle.radius.z) {
          collision_detected = true;
          return;
        }
      } else if (scene_->GetRegistry().has<CircleComponent>(entity)) {
        auto& other_tc = scene_->GetRegistry().get<TransformComponent>(entity);
        BoundingCircle other_circle = BoundingCircle::GetWorldSpherePos(other_tc.Translation(), other_tc.Rotation(), other_tc.Scale());

        const float distance = glm::sqrt(
                                         (circle.position.x - other_circle.position.x) * (circle.position.x - other_circle.position.x) +
                                         (circle.position.y - other_circle.position.y) * (circle.position.y - other_circle.position.y) +
                                         (circle.position.z - other_circle.position.z) * (circle.position.z - other_circle.position.z)
                                         );
                
        if (distance <= circle.radius.x + other_circle.radius.x and
            distance <= circle.radius.y + other_circle.radius.y and
            distance <= circle.radius.z + other_circle.radius.z) {
          collision_detected = true;
          return;
        }
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
    else if (Input::IsKeyPressed(KeyCode::Right))
      translation.x += speed_ * ts;
    else if (Input::IsKeyPressed(KeyCode::Up))
      translation.y += speed_ * ts;
    else if (Input::IsKeyPressed(KeyCode::Down))
      translation.y -= speed_ * ts;
    
    if (HasComponent<QuadComponent>()) {
      auto& tc = GetComponent<TransformComponent>();
      AABB world_aabb = AABB::GetWorldAABBPos(Math::GetTransformMatrix(translation, tc.Rotation(), tc.Scale()));
      
      // If no collision then update the position
      if (!CollisionDetected(world_aabb))
        tc.UpdateTranslation(translation);
    } else if (HasComponent<CircleComponent>()) {
      auto& tc = GetComponent<TransformComponent>();
      BoundingCircle world_circle = BoundingCircle::GetWorldSpherePos(translation, tc.Rotation(), tc.Scale());
      
      // If no collision then update the position
      if (!CollisionDetected(world_circle))
        tc.UpdateTranslation(translation);
    }
  }
  
  void MovementController::RenderGui() {
    ImGui::Text("Speed : %f", speed_);
  }
  
}

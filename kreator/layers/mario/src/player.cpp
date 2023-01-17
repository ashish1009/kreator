//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#include "player.hpp"
#include "camera_controller.h"

namespace mario {
    
  Player::Player(EnttScene* scene)
  : scene_(scene) {
    IK_INFO("Mario", "Creating Mario Player");
    
    player_entity_ = scene_->CreateEntity("Player");
    player_entity_.AddComponent<QuadComponent>();
    
    AABB aabb;
    aabb.min = { -0.5f, -0.5f, -0.5f };
    aabb.max = { 0.5f, 0.5f, 0.5f };
    player_entity_.AddComponent<NativeBodyTypeComponent>(NativeBodyTypeComponent::Type::AABB);
    
    auto& native_script_comp = player_entity_.AddComponent<NativeScriptComponent>([](NativeScriptComponent* sc,
                                                                                    const std::string& script_name) {
      if (script_name == "mario::PlayerController") {
        sc->Bind<mario::PlayerController>();
        return true;
      }
      return false;
    });
    native_script_comp.Bind<mario::PlayerController>();
  }
  
  Player::~Player() {
    IK_WARN("Mario", "Destroying Mario Player");
  }
  
#if MARIO_DEBUG
  void Player::RenderGui() {
    ImGui::Begin("Player Pos");
    ImGui::PushID("Player Pos");
    
    player_entity_.GetComponent<TransformComponent>().RenderGui();
    
    ImGui::PopID();
    ImGui::End();
    
    auto& scrip_name_map = player_entity_.GetComponent<NativeScriptComponent>().scrip_name_map;
    for (auto& [name, script] : scrip_name_map) {
      if (name == "mario::PlayerController") {
        script->RenderGui();
      }
    }

  }
  
  void Player::Reset() {
    player_entity_.GetComponent<TransformComponent>().UpdateTranslation({0, 0, 0});
  }
  
  std::string PlayerController::GetStateString() {
    std::string states = "";
    if (IsState(FreeFalling))
      states += "Freefalling | ";
    return states;
  }
  
#endif

  void PlayerController::RenderGui() {
#if MARIO_DEBUG
    ImGui::Begin("Player Controller");
    ImGui::PushID("Player Controller");

    ImGui::Text("State : %s", GetStateString().c_str());
    
    ImGui::PopID();
    ImGui::End();
#endif
  }
  
  void PlayerController::Update(Timestep ts) {
    if (IsState(FreeFalling))
      Freefall(ts);
#if MOVE
    // Dummy copy of entity y Position
    auto translation = GetComponent<TransformComponent>().Translation();
    if (Input::IsKeyPressed(KeyCode::Left))
      translation.x -= player_data::speed_ * ts;
    if (Input::IsKeyPressed(KeyCode::Right))
      translation.x += player_data::speed_ * ts;

    auto& tc = GetComponent<TransformComponent>();
    const AABB& original_aabb = GetComponent<RigidBodyComponent>().aabb;
    AABB world_aabb = original_aabb.GetWorldPosBoundingBox(Math::GetTransformMatrix(translation,
                                                                                    tc.Rotation(),
                                                                                    tc.Scale()));

    // If no collision then update the position
    if (!CollisionDetected(world_aabb))
      tc.Translation() = translation;
#endif
  }
  
  void PlayerController::Freefall(Timestep ts) {
    // Dummy copy of entity y Position
    auto translation = GetComponent<TransformComponent>().Translation();
    translation.y -= player_data::speed_ * ts;
    
    auto& tc = GetComponent<TransformComponent>();
    AABB world_aabb = AABB::GetWorldAABBPos(Math::GetTransformMatrix(translation,
                                                                                    tc.Rotation(),
                                                                                    tc.Scale()));
    
    // If no collision then update the position
    if (!CollisionDetected(world_aabb))
      tc.UpdateTranslation(translation);
  }
  
  // ------------------------------------------------------------------------
  // Movement controller
  // ------------------------------------------------------------------------
  bool PlayerController::CollisionDetected(const AABB& aabb) {
    bool collision_detected = false;
    // Collistion deteection
    scene_->GetRegistry().view<NativeBodyTypeComponent>().each([&](entt::entity entity, auto& rc)
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
  
  bool PlayerController::IsState(State state_bit) {
    return state_bits_ & state_bit;
  }
  
}

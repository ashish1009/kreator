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
    auto& aabb_comp = player_entity_.AddComponent<RigidBodyComponent>(RigidBodyComponent::Type::AABB);
    aabb_comp.aabb = aabb;
    
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
    const AABB& original_aabb = GetComponent<RigidBodyComponent>().aabb;
    AABB world_aabb = original_aabb.GetWorldAABBPos(Math::GetTransformMatrix(translation,
                                                                                    tc.Rotation(),
                                                                                    tc.Scale()));
    
    // If no collision then update the position
    if (!CollisionDetected(world_aabb))
      tc.UpdateTranslation(translation);
  }
  
  bool PlayerController::IsState(State state_bit) {
    return state_bits_ & state_bit;
  }
  
}

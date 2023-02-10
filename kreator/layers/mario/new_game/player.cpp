//
//  player.cpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#include "player.hpp"

namespace mario {
  
  void Player::Init(std::shared_ptr<EnttScene> scene) {
    scene_ = scene;
    entity_ = scene_->CreateEntity("Player");

    entity_.GetComponent<TransformComponent>().UpdateTranslation({-15, 4, 0.1});
    
    // Quad Component
    auto& qc = entity_.AddComponent<QuadComponent>();

    qc.texture_comp.use = true;
    qc.texture_comp.use_sprite = true;
    qc.texture_comp.linear_edge = false;
    
    qc.texture_comp.component = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), qc.texture_comp.linear_edge);
    qc.texture_comp.sprite = SubTexture::CreateFromCoords(qc.texture_comp.component, {6.0f, 30.0f});
    
    // Rigid Body Component
    auto& rbc = entity_.AddComponent<RigidBodyComponent>();
    rbc.type = b2_dynamicBody;

    // Box Collider
    entity_.AddComponent<BoxColloiderComponent>();

    // Animation
    auto& ac = entity_.AddComponent<AnimationComponent>(qc.texture_comp.component);
    ac.animation = false;
    ac.is_sprite = true;
    ac.sprites.push_back(SubTexture::CreateFromCoords(qc.texture_comp.component, {0.0f, 30.0f}));
    ac.sprites.push_back(SubTexture::CreateFromCoords(qc.texture_comp.component, {1.0f, 30.0f}));
    ac.sprites.push_back(SubTexture::CreateFromCoords(qc.texture_comp.component, {2.0f, 30.0f}));
    
    // Native script
    entity_.AddComponent<NativeScriptComponent>([](NativeScriptComponent* sc,
                                                   const std::string& script_name) {
      if (script_name == "mario::PlayerController") {
        sc->Bind<mario::PlayerController>(10.0f);
        return true;
      }
      return false;
    }).Bind<mario::PlayerController>(10.0f);
  }
  
  PlayerController::PlayerController(float speed) : speed_(speed) {
    
  }
  
  void PlayerController::Update(Timestep ts) {
    auto& tc = GetComponent<TransformComponent>();
    
    if (Input::IsKeyPressed(KeyCode::Left))
      tc.UpdateTranslation_X(tc.Translation().x - (speed_ * ts));
    if (Input::IsKeyPressed(KeyCode::Right))
      tc.UpdateTranslation_X(tc.Translation().x + (speed_ * ts));
  }
  
}

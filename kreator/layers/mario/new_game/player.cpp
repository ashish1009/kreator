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

  }
  
}

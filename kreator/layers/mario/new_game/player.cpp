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
    auto& qc = entity_.AddComponent<QuadComponent>();

    qc.texture_comp.use = true;
    qc.texture_comp.use_sprite = true;
    qc.texture_comp.linear_edge = false;
    
    qc.texture_comp.component = Renderer::GetTexture(AM::ClientAsset("textures/player.png"), qc.texture_comp.linear_edge);
    qc.texture_comp.sprite = SubTexture::CreateFromCoords(qc.texture_comp.component, {6.0f, 30.0f});
  }
  
}

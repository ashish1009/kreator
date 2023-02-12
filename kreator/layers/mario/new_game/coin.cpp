//
//  coin.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "coin.hpp"

namespace mario {
  
  void Coin::CreateBlockCoin(EnttScene* scene, const glm::vec2& pos) {
    static std::shared_ptr<Texture> icons = Renderer::GetTexture(AM::ClientAsset("textures/items.png"), false);
    
    auto coin_entity = scene->CreateEntity("Block Coin");
    coin_entity.GetComponent<TransformComponent>().UpdateTranslation(glm::vec3(pos, 0.1f));
    
    auto & qc = coin_entity.AddComponent<QuadComponent>();
    qc.texture_comp.use = true;
    qc.texture_comp.use_sprite = true;
    qc.texture_comp.linear_edge = false;
    qc.texture_comp.component = icons;
    qc.texture_comp.sprite = SubTexture::CreateFromCoords(icons, {0.0f, 14.0f});
    
    coin_entity.AddComponent<NativeScriptComponent>().Bind<CoinController>();
  }
  
}

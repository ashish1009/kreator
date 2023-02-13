//
//  coin.hpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class CoinController : public ScriptableEntity {
  public:
    void Create(Entity entity) override {
      entity_ = entity;
      printf("%d %x %x \n", (uint32_t)(entity_), entity_.GetScene(), this);
      const auto& tc = entity_.GetComponent<TransformComponent>().Translation();
      top_pos_ = {tc.x, tc.y + 5.0f};
    }
    void Update(Timestep ts) override {
      auto& tc = entity_.GetComponent<TransformComponent>();
      if (tc.Translation().y < top_pos_.y) {
        tc.UpdateTranslation_Y(tc.Translation().y + ts * speed_);
        tc.UpdateScale_X(tc.Scale().x - fmod((2.5f * ts), -1.0f));
      }
      else {
        entity_.GetScene()->DestroyEntity(entity_);
      }
    }
    
  private:
    glm::vec2 top_pos_;
    float speed_ = 8.0f;
  };
  
  class Coin {
  public:
    static void CreateBlockCoin(EnttScene* scene, const glm::vec2& pos);
  };
  
}


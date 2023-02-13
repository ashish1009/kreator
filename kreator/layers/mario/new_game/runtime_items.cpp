//
//  coin.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "runtime_items.hpp"

namespace mario {

  void CoinController::Create(Entity entity) {
    entity_ = entity;
    const auto& tc = entity_.GetComponent<TransformComponent>().Translation();
    top_pos_ = {tc.x, tc.y + 5.0f};
  }
  
  void CoinController::Update(Timestep ts) {
    auto& tc = entity_.GetComponent<TransformComponent>();
    if (tc.Translation().y < top_pos_.y) {
      tc.UpdateTranslation_Y(tc.Translation().y + ts * speed_);
      tc.UpdateScale_X(tc.Scale().x - fmod((2.5f * ts), -1.0f));
    }
    else {
      entity_.GetScene()->DestroyEntity(entity_);
    }
  }

  
}

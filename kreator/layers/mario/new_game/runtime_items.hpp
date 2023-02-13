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
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    
  private:
    glm::vec2 top_pos_;
    float speed_ = 8.0f;
  };
    
}


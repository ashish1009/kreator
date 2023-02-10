//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class PlayerController : public ScriptableEntity {
  public:
    PlayerController(float speed);
    void Update(Timestep ts) override;
    
  private:
    float speed_ = 10.0f;
  };
  
  class Player {
  public:
    void Init(std::shared_ptr<EnttScene> scene);
    
  private:
    std::shared_ptr<EnttScene> scene_;
    Entity entity_;
  };
  
}

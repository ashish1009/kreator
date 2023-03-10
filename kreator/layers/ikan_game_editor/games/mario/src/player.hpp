//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;

  enum class PlayerState {
    Small, Big, Fire, Invicible, Luigi
  };

  class PlayerController : public ScriptableEntity {
  public:
    PlayerController();
  
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void RenderGui() override;

    static PlayerController* Get();
    
  private:
    void CheckOnGround();
    
    bool reset_fixture_ = false;
    bool on_ground_ = false;
    
    float player_width_ = 1.0f, player_height_ = 1.0f;;

    PlayerState player_state_ = PlayerState::Small;

    static PlayerController* instance_;
  };
  
}

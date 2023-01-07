//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

#define MARIO_DEBUG 1

namespace mario {
  
  using namespace ikan;
  
  class PlayerController : public ScriptableEntity {
  public:
    PlayerController() = default;
    
    void Update(Timestep ts) override;
    
  private:
    float speed_ = 25.0f;
  };

  class Player {
  public:
    /// Constructor of mario Player
    Player(EnttScene* scene);
    /// Destructor of mario Player
    ~Player();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(Player);
    
#if MARIO_DEBUG
    void RenderGui();
    void Reset();
#endif
    
  private:
    EnttScene* scene_;
    Entity player_entity_;
  };
  
}

//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 10/03/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;

  class PlayerController : public ScriptableEntity {
  public:
    PlayerController();
    
    static PlayerController* Get();
    
  private:
    static PlayerController* instance_;
  };
  
}

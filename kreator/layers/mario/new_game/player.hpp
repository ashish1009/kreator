//
//  player.hpp
//  kreator
//
//  Created by Ashish . on 10/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class Player {
  public:
    void Init(std::shared_ptr<EnttScene> scene);
    
  private:
    std::shared_ptr<EnttScene> scene_;
  };
  
}

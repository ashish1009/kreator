//
//  run_time_item.hpp
//  kreator
//
//  Created by Ashish . on 13/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  enum class Items {
    Coin
  };
  
  class RuntimeItem {
  public:
    static void CreateBlockCoin(Items item, EnttScene* scene, const glm::vec2& pos);
  };
  
}

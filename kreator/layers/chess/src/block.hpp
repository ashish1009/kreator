//
//  block.hpp
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#pragma once

#include "piece.hpp"

namespace chess {
  
  static constexpr float BlockSize = 4.0f;
  
  /// This structure stores the data of each blockl of chess. Data like
  struct Block {
    Position row = -1, col = -1;
    std::shared_ptr<Piece> piece = nullptr;
  };
  
}

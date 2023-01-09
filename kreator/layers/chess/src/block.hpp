//
//  block.hpp
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#pragma once

#include "piece.hpp"

namespace chess {
  
  /// This structure stores the data of each blockl of chess. Data like
  struct Block {
    Position x = -1, y = -1;
    std::shared_ptr<Piece> piece = nullptr;
  };
  
}

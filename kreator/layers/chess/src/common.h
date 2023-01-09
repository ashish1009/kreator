//
//  common.h
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#pragma once

#define CHESS_DEBUG 0

namespace chess {
  
  using Position = int32_t;
  
  static constexpr uint32_t MaxRows = 8;
  static constexpr uint32_t MaxCols = 8;
  
  enum class Color {
    None, Black, White
  };
  
}

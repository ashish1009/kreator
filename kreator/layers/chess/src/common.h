//
//  common.h
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#pragma once

#define CHESS_DEBUG 1

namespace chess {
  
  using Position = int32_t;
  
  static constexpr uint32_t MaxRows = 8;
  static constexpr uint32_t MaxCols = 8;
  
  static constexpr glm::vec4 BlackColor = { 0, 0, 0, 1 };
  static constexpr glm::vec4 WhiteColor = { 1, 1, 1, 1 };
  
  enum class Color {
    None, Black, White
  };
  
}

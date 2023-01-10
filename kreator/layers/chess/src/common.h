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
  
  static constexpr glm::vec4 BlackColor = { 0.39f, 0.21f, 0.21f, 1 };
  static constexpr glm::vec4 WhiteColor = { 0.47f, 0.59f, 0.27f, 1 };
  
  enum class Color {
    None, Black, White
  };
  
}

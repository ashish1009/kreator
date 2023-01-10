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
  
  static constexpr glm::vec4 BlackColor = { 0.04f, 0.23f, 0.04f, 1 };
  static constexpr glm::vec4 WhiteColor = { 0.57f, 0.80f, 0.84f, 1 };
  
  enum class Color {
    None, Black, White
  };
  
}

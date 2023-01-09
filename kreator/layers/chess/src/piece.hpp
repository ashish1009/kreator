//
//  piece.hpp
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#pragma once

#include "common.h"

namespace chess {
  
  /// Base class for all piece. Stores the following data
  ///  - Type of piece
  ///  - Color of piece
  ///  - Entity Data (Texture)
  ///  - Position of piece
  class Piece {
  public:
    // Enum to store Type of piece
    enum class Type {
      None, Pawn, King, Queen, Bishop, Knight, Rook
    };
    
  private:
    Type type_ = Type::None;
    Position x_ = -1, y_ = -1;
    Color color_ = Color::None;
  };
  
}

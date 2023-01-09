//
//  piece.hpp
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#pragma once

#include "common.h"

namespace chess {
  
  struct StartPosition {
    static constexpr Position King = 3;
    static constexpr Position Queen = 4;
    static constexpr Position Rook_1 = 0;
    static constexpr Position Rook_2 = 7;
    static constexpr Position Knight_1 = 1;
    static constexpr Position Knight_2 = 6;
    static constexpr Position Bishop_1 = 2;
    static constexpr Position Bishop_2 = 5;
  };
  
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
    
    virtual ~Piece() = default;
    
    /// This function create the pirce based on the type of piece
    /// - Parameter type: piece type
    static std::shared_ptr<Piece> Create(Type type);
    
    /// This function returns the type of piece at position x, y in start game
    /// - Parameters:
    ///   - start_x: x position
    ///   - start_y: y position
    static Type GetStartPieceType(Position start_x, Position start_y);
    
  private:
    Type type_ = Type::None;
    Position x_ = -1, y_ = -1;
    Color color_ = Color::None;
  };
  
  /// Class to store the pawn data and funtion
  class Pawn : public Piece {
  public:
  };

  /// Class to store the King data and funtion
  class King : public Piece {
  public:
  };

  /// Class to store the queen data and funtion
  class Queen : public Piece {
  public:
  };

  /// Class to store the Bishop data and funtion
  class Bishop : public Piece {
  public:
  };

  /// Class to store the Knight data and funtion
  class Knight : public Piece {
  public:
  };

  /// Class to store the Rook data and funtion
  class Rook : public Piece {
  public:
  };

}

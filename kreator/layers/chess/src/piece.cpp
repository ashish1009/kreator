//
//  piece.cpp
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#include "piece.hpp"

namespace chess {
  
  static Piece::Type GetYPosPiece(Position start_y) {
    if (start_y == StartPosition::King) return Piece::Type::King;
    else if (start_y == StartPosition::Queen)  return Piece::Type::Queen;
    else if (start_y == StartPosition::Bishop_1)  return Piece::Type::Bishop;
    else if (start_y == StartPosition::Bishop_2)  return Piece::Type::Bishop;
    else if (start_y == StartPosition::Rook_1)  return Piece::Type::Rook;
    else if (start_y == StartPosition::Rook_2)  return Piece::Type::Rook;
    else if (start_y == StartPosition::Knight_1)  return Piece::Type::Knight;
    else if (start_y == StartPosition::Knight_2)  return Piece::Type::Knight;
    else IK_ASSERT(false);
  }
  
  Piece::Type Piece::GetStartPieceType(Position start_x, Position start_y) {
    if (start_x == 0 or start_x == 7) return GetYPosPiece(start_y);
    else if (start_x == 1 or start_x == 6)
      return Piece::Type::Pawn;
    else IK_ASSERT(false);
  }
  
  std::shared_ptr<Piece> Piece::Create(Piece::Type type) {
    switch (type) {
      case Piece::Type::Pawn: return std::make_shared<Pawn>();
      case Piece::Type::King: return std::make_shared<King>();
      case Piece::Type::Queen: return std::make_shared<Queen>();
      case Piece::Type::Bishop: return std::make_shared<Bishop>();
      case Piece::Type::Knight: return std::make_shared<Knight>();
      case Piece::Type::Rook: return std::make_shared<Rook>();
      case Piece::Type::None:
      default:
        IK_ASSERT(false);
    }
  }
  
}

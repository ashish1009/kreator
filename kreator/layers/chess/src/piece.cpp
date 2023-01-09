//
//  piece.cpp
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#include "piece.hpp"

namespace chess {
  
  static Piece::Type GetYPosPiece(Position col) {
    if (col == StartPosition::King) return Piece::Type::King;
    else if (col == StartPosition::Queen)  return Piece::Type::Queen;
    else if (col == StartPosition::Bishop_1)  return Piece::Type::Bishop;
    else if (col == StartPosition::Bishop_2)  return Piece::Type::Bishop;
    else if (col == StartPosition::Rook_1)  return Piece::Type::Rook;
    else if (col == StartPosition::Rook_2)  return Piece::Type::Rook;
    else if (col == StartPosition::Knight_1)  return Piece::Type::Knight;
    else if (col == StartPosition::Knight_2)  return Piece::Type::Knight;
    else IK_ASSERT(false);
  }
  
  Piece::Type GetStartPieceType(Position row, Position col) {
    if (row == 0 or row == 7) return GetYPosPiece(col);
    else if (row == 1 or row == 6)
      return Piece::Type::Pawn;
    else IK_ASSERT(false);
  }
  
  Color GetPieceStartColor(Position row) {
    if (row == 0 or row == 1) return Color::Black;
    else if (row == 6 or row == 7) return Color::White;
    else IK_ASSERT(false);
  }
  
  std::shared_ptr<Piece> Piece::Create(Position row, Position col) {
    Color color = GetPieceStartColor(row);
    switch (GetStartPieceType(row, col)) {
      case Piece::Type::Pawn: return std::make_shared<Pawn>(color, row, col);
//      case Piece::Type::King: return std::make_shared<King>(color);
//      case Piece::Type::Queen: return std::make_shared<Queen>(color);
//      case Piece::Type::Bishop: return std::make_shared<Bishop>(color);
//      case Piece::Type::Knight: return std::make_shared<Knight>(color);
//      case Piece::Type::Rook: return std::make_shared<Rook>(color);
//      case Piece::Type::None:
//      default:
//        IK_ASSERT(false);
    }
    return nullptr;
  }
  
  // -------------------------------------------
  // Pieces
  // -------------------------------------------
  
  Piece::Piece(Piece::Type type, Color color, Position row, Position col)
  : type_(type), row_(row), col_(col), color_(color) {
    
  }

  Pawn::Pawn(Color color, Position row, Position col)
  : Piece(Piece::Type::Pawn, color, row, col) { }
  
//  King::King(Color color) {
//    color_ = color;
//    type_ = Piece::Type::King;
//
//  }
//  King::~King() {
//
//  }
//
//  Queen::Queen(Color color) {
//    color_ = color;
//    type_ = Piece::Type::Queen;
//
//  }
//  Queen::~Queen() {
//
//  }
//
//  Bishop::Bishop(Color color) {
//    color_ = color;
//    type_ = Piece::Type::Bishop;
//
//  }
//  Bishop::~Bishop() {
//
//  }
//
//  Knight::Knight(Color color) {
//    color_ = color;
//    type_ = Piece::Type::Knight;
//
//  }
//  Knight::~Knight() {
//
//  }
//
//  Rook::Rook(Color color) {
//    color_ = color;
//    type_ = Piece::Type::Rook;
//
//  }
//  Rook::~Rook() {
//
//  }
}

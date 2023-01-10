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
    else return Piece::Type::None;
  }
  
  Color GetPieceStartColor(Position row) {
    if (row == 0 or row == 1) return Color::Black;
    else if (row == 6 or row == 7) return Color::White;
    else return Color::None;
  }
  
  std::shared_ptr<Piece> Piece::Create(Position row, Position col) {
    Color color = GetPieceStartColor(row);
    switch (GetStartPieceType(row, col)) {
      case Piece::Type::Pawn: return std::make_shared<Pawn>(color, row, col);
      case Piece::Type::King: return std::make_shared<King>(color, row, col);
      case Piece::Type::Queen: return std::make_shared<Queen>(color, row, col);
      case Piece::Type::Bishop: return std::make_shared<Bishop>(color, row, col);
      case Piece::Type::Knight: return std::make_shared<Knight>(color, row, col);
      case Piece::Type::Rook: return std::make_shared<Rook>(color, row, col);
      case Piece::Type::None: return nullptr;
      default:
        IK_ASSERT(false);
    }
    return nullptr;
  }
  
  // -------------------------------------------
  // Pieces
  // -------------------------------------------
  std::string Piece::GetColorStr() const {
    if (color_ == Color::White) return "White";
    else if (color_ == Color::Black) return "Black";
    else IK_ASSERT(false);
  }
  
  Piece::Piece(Piece::Type type, Color color, Position row, Position col)
  : type_(type), row_(row), col_(col), color_(color) {
    switch (type_) {
      case Piece::Type::Pawn: name_ = "Pawn"; break;
      case Piece::Type::King: name_ = "King"; break;
      case Piece::Type::Queen: name_ = "Queen"; break;
      case Piece::Type::Bishop: name_ = "Bishop"; break;
      case Piece::Type::Knight: name_ = "Knight"; break;
      case Piece::Type::Rook: name_ = "Rook"; break;
      case Piece::Type::None: name_ = "Empty"; break;
      default:
        IK_ASSERT(false);
    }
  }
  
  Pawn::Pawn(Color color, Position row, Position col)
  : Piece(Piece::Type::Pawn, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "pawn.png"));
  }
  
  King::King(Color color, Position row, Position col)
  : Piece(Piece::Type::King, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "king.png"));
  }
  
  Queen::Queen(Color color, Position row, Position col)
  : Piece(Piece::Type::Queen, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "queen.png"));
  }
  
  Bishop::Bishop(Color color, Position row, Position col)
  : Piece(Piece::Type::Bishop, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "bishop.png"));
  }
  
  Knight::Knight(Color color, Position row, Position col)
  : Piece(Piece::Type::Knight, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "knight.png"));
  }
  
  Rook::Rook(Color color, Position row, Position col)
  : Piece(Piece::Type::Rook, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "rook.png"));
  }
  
}

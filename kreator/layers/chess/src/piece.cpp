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
  
  Pawn::Direction GetpawnDirection(Position row) {
    if (row == 0 or row == 1) return Pawn::Direction::Up;
    else if (row == 6 or row == 7) return Pawn::Direction::Down;
    IK_ASSERT(false);
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
  
  // --------------------------
  // Possible Move Validation
  // --------------------------
  struct PossibleMoves {
    PossibleMoves(Position start_row, Position start_col, PossiblePositions& positions)
    : start_row_(start_row), start_col_(start_col), positions_(positions) {}
    
    /// This function update the possible positions in Straight Up direction
    /// - Parameters:
    ///   - start_row: start row of piece
    ///   - start_col: start column of piece
    ///   - limit: range row that need to be validate
    ///   - positions: positions reference to be updated
    void Upward(Position limit) {
      Position row = start_row_;
      while (row++ < limit) {
        positions_.push_back(std::make_pair(row, start_col_));
      }
    }
    
    /// This function update the possible positions in Straight Down direction
    /// - Parameters:
    ///   - start_row: start row of piece
    ///   - start_col: start column of piece
    ///   - limit: range row that need to be validate
    ///   - positions: positions reference to be updated
    void Downward(Position limit) {
      Position row = start_row_;
      while (row-- > limit) {
        positions_.push_back(std::make_pair(row, start_col_));
      }
    }

    
    Position start_row_;
    Position start_col_;
    PossiblePositions& positions_;
  };
  
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
  : Piece(Piece::Type::Pawn, color, row, col), direction_(GetpawnDirection(row)) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "pawn.png"));
  }
  
  PossibleMoveBlocks Pawn::GetPossibleMovePositions() {
    PossibleMoveBlocks result;
    
    PossibleMoves possible_moves(row_, col_, result.empty_blocks_);
    
    if (direction_ == Direction::Up) {
      if (row_ < MaxRows - 1 and row_ >= 0) {
        possible_moves.Upward(row_ + 1);
        
        result.piece_blocks_.push_back(std::make_pair(row_ + 1, col_ + 1));
        result.piece_blocks_.push_back(std::make_pair(row_ + 1, col_ - 1));
      }
    } else if (direction_ == Direction::Down) {
      if (row_ < MaxRows and row_ > 0) {
        possible_moves.Downward(row_ - 1);
        
        result.piece_blocks_.push_back(std::make_pair(row_ - 1, col_ + 1));
        result.piece_blocks_.push_back(std::make_pair(row_ - 1, col_ - 1));
      }
    } else {
      IK_ASSERT(false);
    }
    
    return result;
  }
  
  King::King(Color color, Position row, Position col)
  : Piece(Piece::Type::King, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "king.png"));
  }

  PossibleMoveBlocks King::GetPossibleMovePositions() {
    PossibleMoveBlocks result;
    return result;
  }

  Queen::Queen(Color color, Position row, Position col)
  : Piece(Piece::Type::Queen, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "queen.png"));
  }

  PossibleMoveBlocks Queen::GetPossibleMovePositions() {
    PossibleMoveBlocks result;
    return result;
  }

  Bishop::Bishop(Color color, Position row, Position col)
  : Piece(Piece::Type::Bishop, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "bishop.png"));
  }

  PossibleMoveBlocks Bishop::GetPossibleMovePositions() {
    PossibleMoveBlocks result;
    return result;
  }

  Knight::Knight(Color color, Position row, Position col)
  : Piece(Piece::Type::Knight, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "knight.png"));
  }

  PossibleMoveBlocks Knight::GetPossibleMovePositions() {
    PossibleMoveBlocks result;
    return result;
  }

  Rook::Rook(Color color, Position row, Position col)
  : Piece(Piece::Type::Rook, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "rook.png"));
  }

  PossibleMoveBlocks Rook::GetPossibleMovePositions() {
    PossibleMoveBlocks result;
    return result;
  }

}

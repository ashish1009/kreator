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
    else if (row == 1)
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
  struct PossibleMovesValidator {
    PossibleMovesValidator(Position start_row, Position start_col, PossibleMoves& moves)
    : start_row_(start_row), start_col_(start_col), moves_(moves) {}
    
    /// This function update the possible positions in Straight Up direction
    /// - Parameter row_limit: range row that need to be validate
    void Upward(Position row_limit) {
      if (start_row_ >= MaxRows - 1 or start_row_ < 0)
        return;
      
      Position row = start_row_;
      while (row++ < row_limit) {
        moves_.up.push_back(std::make_pair(row, start_col_));
      }
    }
    
    /// This function update the possible positions in Straight Down direction
    /// - Parameter row_limit: range row that need to be validate
    void Downward(Position row_limit) {
      if (start_row_ >= MaxRows or start_row_ <= 0)
        return;
      
      Position row = start_row_;
      while (row-- > row_limit) {
        moves_.down.push_back(std::make_pair(row, start_col_));
      }
    }
    
    /// This function update the possible positions in Straight Right direction
    /// - Parameter col_limit: range row that need to be validate
    void Right(Position col_limit) {
      if (start_col_ >= MaxCols - 1 or start_col_ < 0)
        return;
      
      Position col = start_col_;
      while (col++ > col_limit) {
        moves_.right.push_back(std::make_pair(start_row_, col));
      }
    }
    
    /// This function update the possible positions in Straight Left direction
    /// - Parameter col_limit: range row that need to be validate
    void Left(Position col_limit) {
      if (start_col_ >= MaxCols or start_col_ <= 0)
        return;

      Position col = start_col_;
      while (col-- > col_limit) {
        moves_.left.push_back(std::make_pair(start_row_, col));
      }
    }
    
    /// This function update the possible positions in Diaginal Up Right direction
    /// - Parameters:
    ///   - row_limit: range row that need to be validate
    ///   - col_limit: range column that need to be validate
    void UpRight(Position row_limit, Position col_limit) {
      if (start_row_ >= MaxRows - 1 or start_row_ < 0 or start_col_ >= MaxCols - 1 or start_col_ < 0)
        return;
      
      Position row = start_row_;
      Position col = start_col_;
      while (row++ < row_limit and col++ < col_limit) {
        moves_.up_right.push_back(std::make_pair(row, col));
      }
    }
    
    /// This function update the possible positions in Diaginal Up Left direction
    /// - Parameters:
    ///   - row_limit: range row that need to be validate
    ///   - col_limit: range column that need to be validate
    void UpLeft(Position row_limit, Position col_limit) {
      if (start_row_ >= MaxRows - 1 or start_row_ < 0 or start_col_ >= MaxCols or start_col_ <= 0)
        return;

      Position row = start_row_;
      Position col = start_col_;
      while (row++ < row_limit and col-- > col_limit) {
        moves_.up_left.push_back(std::make_pair(row, col));
      }
    }
    
    /// This function update the possible positions in Diagonal Down Right direction
    /// - Parameters:
    ///   - row_limit: range row that need to be validate
    ///   - col_limit: range column that need to be validate
    void DownRight(Position row_limit, Position col_limit) {
      if (start_row_ >= MaxRows or start_row_ <= 0 or start_col_ >= MaxCols - 1 or start_col_ < 0)
        return;

      Position row = start_row_;
      Position col = start_col_;
      while (row-- > row_limit and col++ < col_limit) {
        moves_.down_right.push_back(std::make_pair(row, col));
      }
    }
    
    /// This function update the possible positions in Diagonal Down Left direction
    /// - Parameters:
    ///   - row_limit: range row that need to be validate
    ///   - col_limit: range column that need to be validate
    void DownLeft(Position row_limit, Position col_limit) {
      if (start_row_ >= MaxRows or start_row_ <= 0 or start_col_ >= MaxCols or start_col_ <= 0)
        return;
      
      Position row = start_row_;
      Position col = start_col_;
      while (row-- > row_limit and col-- > col_limit) {
        moves_.down_left.push_back(std::make_pair(row, col));
      }
    }

    Position start_row_;
    Position start_col_;
    PossibleMoves& moves_;
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
  
  PossibleMoves Pawn::GetPossibleMovePositions() {
    PossibleMoves result;
    PossibleMovesValidator possible_moves(row_, col_, result);
    
    if (direction_ == Direction::Up) {
      possible_moves.Upward(row_ + 1);
    } else if (direction_ == Direction::Down) {
      possible_moves.Downward(row_ - 1);
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

  PossibleMoves King::GetPossibleMovePositions() {
    PossibleMoves result;
    PossibleMovesValidator possible_moves(row_, col_, result);
    
    possible_moves.Upward(row_ + 1);
    possible_moves.Downward(row_ - 1);

    possible_moves.Right(col_ + 1);
    possible_moves.Left(col_ - 1);

    possible_moves.UpLeft(row_ + 1, col_ -1);
    possible_moves.UpRight(row_ + 1, col_ + 1);
    
    possible_moves.DownLeft(row_ - 1, col_ - 1);
    possible_moves.DownRight(row_ - 1, col_ + 1);

    return result;
  }

  Queen::Queen(Color color, Position row, Position col)
  : Piece(Piece::Type::Queen, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "queen.png"));
  }

  PossibleMoves Queen::GetPossibleMovePositions() {
    PossibleMoves result;
    PossibleMovesValidator possible_moves(row_, col_, result);
    
    possible_moves.Upward(MaxRows - 1);
    possible_moves.Downward(0);

    possible_moves.Right(MaxCols - 1);
    possible_moves.Left(0);

    possible_moves.UpLeft(MaxRows - 1, 0);
    possible_moves.UpRight(MaxRows - 1, MaxCols - 1);

    possible_moves.DownLeft(0, 0);
    possible_moves.DownRight(0, MaxCols - 1);

    return result;
  }

  Bishop::Bishop(Color color, Position row, Position col)
  : Piece(Piece::Type::Bishop, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "bishop.png"));
  }

  PossibleMoves Bishop::GetPossibleMovePositions() {
    PossibleMoves result;
    PossibleMovesValidator possible_moves(row_, col_, result);
    
    possible_moves.UpLeft(MaxRows - 1, 0);
    possible_moves.UpRight(MaxRows - 1, MaxCols - 1);
    
    possible_moves.DownLeft(0, 0);
    possible_moves.DownRight(0, MaxCols - 1);

    return result;
  }

  Rook::Rook(Color color, Position row, Position col)
  : Piece(Piece::Type::Rook, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "rook.png"));
  }
  
  PossibleMoves Rook::GetPossibleMovePositions() {
    PossibleMoves result;
    PossibleMovesValidator possible_moves(row_, col_, result);
    
    possible_moves.Upward(MaxRows - 1);
    possible_moves.Downward(0);
    
    possible_moves.Right(MaxCols - 1);
    possible_moves.Left(0);
    
    return result;
  }
  
  Knight::Knight(Color color, Position row, Position col)
  : Piece(Piece::Type::Knight, color, row, col) {
    std::string texture_path = color == Color::Black ? "textures/black/" : "textures/white/";
    texture_ = Renderer::GetTexture(AM::ClientAsset(texture_path + "knight.png"));
  }

  PossibleMoves Knight::GetPossibleMovePositions() {
    PossibleMoves result;
    PossibleMovesValidator possible_moves(row_, col_, result);
    return result;
  }

}

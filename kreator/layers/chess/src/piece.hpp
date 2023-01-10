//
//  piece.hpp
//  kreator
//
//  Created by Ashish . on 09/01/23.
//

#pragma once

#include "common.h"

namespace chess {
  
  using namespace ikan;
  
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
  
  using PossiblePositions = std::vector<std::pair<Position, Position>>;
      
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
    
    /// Cosntructor of base piece class
    /// - Parameters:
    ///   - type: type of piece
    ///   - color: color of piece
    ///   - row: row of position
    ///   - col: col position
    Piece(Piece::Type type, Color color, Position row, Position col);
    virtual ~Piece() = default;
    
    /// This function returns the possible moves of a piece (vectore of pair of row and columns)
    [[nodiscard]] virtual PossiblePositions GetPossibleMovePositions() = 0;
    
    /// This function returns the color name string
    std::string GetColorStr() const;
    /// This function returns the type of piece
    Type GetType() const { return type_; }
    /// This function return the name string og piece
    const std::string& GetName() const { return name_; }
    /// This function return the Row of piece
    Position Row() const { return row_; }
    /// This function return the column of piece
    Position Col() const { return col_; }
    /// This function returns the texture of piece
    std::shared_ptr<Texture> GetTexture() { return texture_; }
    
    /// This function create the pirce based on the type of piece
    /// - Parameters:
    ///   - row: x position
    ///   - col: y position
    static std::shared_ptr<Piece> Create(Position row, Position col);

  protected:
    Type type_ = Type::None;
    Position row_ = -1, col_ = -1;
    Color color_ = Color::None;
    std::shared_ptr<Texture> texture_;
    std::string name_;
    Entity* entity_;
  };
  
  /// Class to store the pawn data and funtion
  class Pawn : public Piece {
  public:
    // Direction of pawn as it can move only in one direction
    enum class Direction : uint8_t { Up, Down };
    
    /// Cosntructor of base piece class
    /// - Parameters:
    ///   - type: type of piece
    ///   - color: color of piece
    ///   - row: row of position
    ///   - col: col position
    Pawn(Color color, Position row, Position col);
    ~Pawn() = default;

    /// This function returns the possible moves of a piece (vectore of pair of row and columns)
    PossiblePositions GetPossibleMovePositions() override;
    
  private:
    Direction direction_;
  };

  /// Class to store the King data and funtion
  class King : public Piece {
  public:
    /// Cosntructor of base piece class
    /// - Parameters:
    ///   - type: type of piece
    ///   - color: color of piece
    ///   - row: row of position
    ///   - col: col position
    King(Color color, Position row, Position col);
    ~King() = default;

    /// This function returns the possible moves of a piece (vectore of pair of row and columns)
    PossiblePositions GetPossibleMovePositions() override;
  };

  /// Class to store the queen data and funtion
  class Queen : public Piece {
  public:
    /// Cosntructor of base piece class
    /// - Parameters:
    ///   - type: type of piece
    ///   - color: color of piece
    ///   - row: row of position
    ///   - col: col position
    Queen(Color color, Position row, Position col);
    ~Queen() = default;

    /// This function returns the possible moves of a piece (vectore of pair of row and columns)
    PossiblePositions GetPossibleMovePositions() override;
  };

  /// Class to store the Bishop data and funtion
  class Bishop : public Piece {
  public:
    /// Cosntructor of base piece class
    /// - Parameters:
    ///   - type: type of piece
    ///   - color: color of piece
    ///   - row: row of position
    ///   - col: col position
    Bishop(Color color, Position row, Position col);
    ~Bishop() = default;

    /// This function returns the possible moves of a piece (vectore of pair of row and columns)
    PossiblePositions GetPossibleMovePositions() override;
  };

  /// Class to store the Knight data and funtion
  class Knight : public Piece {
  public:
    /// Cosntructor of base piece class
    /// - Parameters:
    ///   - type: type of piece
    ///   - color: color of piece
    ///   - row: row of position
    ///   - col: col position
    Knight(Color color, Position row, Position col);
    ~Knight() = default;

    /// This function returns the possible moves of a piece (vectore of pair of row and columns)
    PossiblePositions GetPossibleMovePositions() override;
  };

  /// Class to store the Rook data and funtion
  class Rook : public Piece {
  public:
    /// Cosntructor of base piece class
    /// - Parameters:
    ///   - type: type of piece
    ///   - color: color of piece
    ///   - row: row of position
    ///   - col: col position
    Rook(Color color, Position row, Position col);
    ~Rook() = default;

    /// This function returns the possible moves of a piece (vectore of pair of row and columns)
    PossiblePositions GetPossibleMovePositions() override;
  };

}

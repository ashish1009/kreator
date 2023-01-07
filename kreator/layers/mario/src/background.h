//
//  background.h
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  struct BackgroudData {
    // Sprite texture for background tiles
    std::shared_ptr<Texture> tile_sprite;
    
    // Map of subtexture to the Char (defined in s_MapTiles) of their corresponding tile.
    std::unordered_map<char, std::shared_ptr<SubTexture>> tiles_char_map;
    
    BackgroudData(std::shared_ptr<Texture> sprite_iamge) {
      // Create the tile sprite
      tile_sprite = sprite_iamge;

      // Setup all the tile map
      tiles_char_map['G'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 27.0f });
      tiles_char_map['X'] = SubTexture::CreateFromCoords(tile_sprite, { 17.0f, 27.0f });
      tiles_char_map['B'] = SubTexture::CreateFromCoords(tile_sprite, { 24.0f, 27.0f });
      tiles_char_map['b'] = SubTexture::CreateFromCoords(tile_sprite, { 27.0f, 27.0f });
      tiles_char_map['S'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 26.0f });
      tiles_char_map['-'] = SubTexture::CreateFromCoords(tile_sprite, { 15.0f, 18.5f });
      tiles_char_map['Y'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 19.0f }, { 2.0f, 1.0f });
      tiles_char_map['!'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 18.0f }, { 2.0f, 1.0f });
      tiles_char_map['<'] = SubTexture::CreateFromCoords(tile_sprite, { 11.0f, 18.0f });
      tiles_char_map['v'] = SubTexture::CreateFromCoords(tile_sprite, { 12.0f, 18.0f });
      tiles_char_map['>'] = SubTexture::CreateFromCoords(tile_sprite, { 13.0f, 18.0f });
      tiles_char_map['{'] = SubTexture::CreateFromCoords(tile_sprite, { 8.0f, 19.0f });
      tiles_char_map['}'] = SubTexture::CreateFromCoords(tile_sprite, { 10.0f, 19.0f });
      tiles_char_map['*'] = SubTexture::CreateFromCoords(tile_sprite, { 9.0f, 19.0f });
      tiles_char_map['1'] = SubTexture::CreateFromCoords(tile_sprite, { 8.0f, 18.0f });
      tiles_char_map['2'] = SubTexture::CreateFromCoords(tile_sprite, { 9.0f, 18.0f });
      tiles_char_map['3'] = SubTexture::CreateFromCoords(tile_sprite, { 10.0f, 18.0f });
      tiles_char_map['('] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 0.0f }, { 1.0f, 2.0f });
      tiles_char_map['^'] = SubTexture::CreateFromCoords(tile_sprite, { 1.0f, 0.0f }, { 1.0f, 2.0f });
      tiles_char_map[')'] = SubTexture::CreateFromCoords(tile_sprite, { 2.0f, 0.0f }, { 1.0f, 2.0f });
      tiles_char_map['.'] = SubTexture::CreateFromCoords(tile_sprite, { 19.0f, 27.0f });
      tiles_char_map['u'] = SubTexture::CreateFromCoords(tile_sprite, { 20.0f, 26.0f });
      tiles_char_map['o'] = SubTexture::CreateFromCoords(tile_sprite, { 21.0f, 26.0f });
      tiles_char_map['|'] = SubTexture::CreateFromCoords(tile_sprite, { 21.0f, 27.0f });
      tiles_char_map['l'] = SubTexture::CreateFromCoords(tile_sprite, { 20.0f, 27.0f });
      tiles_char_map['r'] = SubTexture::CreateFromCoords(tile_sprite, { 22.0f, 27.0f });
    }
  };

  /* --------------------------------------------------------------------------------------
   Scene String. Each char represnet a tile described below
   0 : END OF LINE TO GET NUMBER OF ELEMENT IN A ROW
   G : Ground
   | : Castel Brick
   o : Castel Gate
   u : castel Gate Domb
   . : Castel Domb
   l : Castel Windlow Left
   r : Castel Window Right
   S : Steps
   - : Bridge
   ! : Pipe Base
   Y : Pipe Head
   X : Bricks
   B : Bonus
   b : Used Bonus
   ( : Cloud (
   ^ : Cloud ^
   ) : Cloud )
   < : Grass <
   v : Grass v
   > : Grass >
   { : Forest {
   } : Forest }
   * : Forest *
   1 : Forest 1
   2 : Forest 2
   3 : Forest 3
   / -------------------------------------------------------------------------------------- */
  // For now making number of row even to make position of y integral
  // will move to level folder/class
  static std::string map_tile_pattern =
  "                                                                                                                                                              0"
  "                                                                                                                                                              0"
  "                                                                           (^)                                       (^^)                                     0"
  "       (^)                  (^^^)                                                                                                                    .        0"
  "                                                                                                                                                    ...       0"
  "                                                                                                                                                    |u|       0"
  "                                                                                                                                                    |o|       0"
  "                                                                                                                                                  .......     0"
  "                   B                  XXXX                             B            XBX                                                           |u|r|u|     0"
  "                                                                                                                                                  |o|||o|     0"
  "                                                                                                                                                ...........   0"
  "                                                                                                                                                |||||||||||   0"
  "                                                                                                                                                |l|u|r|u|l|   0"
  "                 XBXBX              XBXXXXBX                           S           XXXXX               B                                        |||o|||o|||   0"
  "                                                    Y                 SS                                                   S                  ............... 0"
  "                                                    !                SSS                                                  SS                  ||||||||||||||| 0"
  "                              Y                     !               SSSS                               Y                 SSS                  |u||u||u||u||u| 0"
  "               <v>      S     !       S  S          !     <vv>     SSSSS                    <vvv>      !          <     SSSS         <v>      |o||o||o||o||o| 0"
  "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG--------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
  "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG                    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
  ;
  
  
  /// This function returns the entity name from Char code
  /// - Parameter type: Char of tile map
  static std::string GetEntityNameFromChar(char type) {
    switch(type) {
      case 'G' : return "Ground"; break;
        
      case '|' : return "Castel Brick";        break;
      case 'o' : return "Castel Gate";         break;
      case 'u' : return "castel Gate Domb";    break;
      case '.' : return "Castel Domb";         break;
      case 'l' : return "Castel Windlow Left"; break;
      case 'r' : return "Castel Window Right"; break;
        
      case 'S' : return "Steps";     break;
      case '-' : return "Bridge";    break;
      case '!' : return "Pipe Base"; break;
      case 'Y' : return "Pipe";      break;
      case 'X' : return "Bricks";    break;
      case 'B' : return "Bonus";     break;
      case 'b' : return "UsedBonus"; break;
        
      case '<' : return "Grass <";   break;
      case 'v' : return "Grass v";   break;
      case '>' : return "Grass >>";  break;
        
      case '(' : return "Cloud (";    break;
      case '^' : return "Cloud ^";    break;
      case ')' : return "Cloud )>";   break;
        
      case '}' : return "Grass }";    break;
      case '{' : return "Grass {";    break;
      case '*' : return "Grass *";    break;
      case '1' : return "Grass 1";    break;
      case '2' : return "Grass 2";    break;
      case '3' : return "Grass 3";    break;
    };
    IK_ASSERT(false, "Invalid Type");
  }
  
}

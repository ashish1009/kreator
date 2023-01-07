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
    
    BackgroudData(EnttScene* scene, std::shared_ptr<Texture> sprite_iamge)
    : scene_(scene) {
      // Create the tile sprite
      tile_sprite = sprite_iamge;

      StoreTiles();
      CreateEntities();
    }
    
    void StoreTiles();
    void CreateEntities();
    
  private:
    EnttScene* scene_;
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
  "                                                                                                                                                                                            0"
  "                                                                                                                                                                                            0"
  "                                                                                                         (^)                                       (^^)                                     0"
  "                                     (^)                  (^^^)                                                                                                                    .        0"
  "                                                                                                                                                                                  ...       0"
  "                                                                                                                                                                                  |u|       0"
  "                                                                                                                                                                                  |o|       0"
  "                                                                                                                                                                                .......     0"
  "                                                 B                  XXXX                             B            XBX                                                           |u|r|u|     0"
  "                                                                                                                                                                                |o|||o|     0"
  "                                                                                                                                                                              ...........   0"
  "                                                                                                                                                                              |||||||||||   0"
  "                                                                                                                                                                              |l|u|r|u|l|   0"
  "                                               XBXBX              XBXXXXBX                           S           XXXXX               B                                        |||o|||o|||   0"
  "                                                                                  Y                 SS                                                   S                  ............... 0"
  "                                                                                  !                SSS                         *                        SS                  ||||||||||||||| 0"
  "                                                            Y                     !               SSSS                        {1}    Y                 SSS                  |u||u||u||u||u| 0"
  "                                             <v>      S     !       S  S          !     <vv>     SSSSS                       {213}   !                SSSS         <v>      |o||o||o||o||o| 0"
  "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG--------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
  "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG                    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
  ;
  
}

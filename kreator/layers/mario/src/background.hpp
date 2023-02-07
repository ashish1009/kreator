//
//  background.h
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once
#include "common.h"

namespace mario {
  
  using namespace ikan;
  
  struct BackgroudData {
    // Sprite texture for background tiles
    std::shared_ptr<Texture> tile_sprite;
    
    // Map of subtexture to the Char (defined in s_MapTiles) of their corresponding tile.
    std::unordered_map<char, std::shared_ptr<SubTexture>> tiles_char_map;
    std::unordered_map<char, std::shared_ptr<Texture>> texture_char_map;

    BackgroudData(EnttScene* tile_scene, EnttScene* texture_scene, std::shared_ptr<Texture> sprite_iamge)
    : tile_scene_(tile_scene), texture_scene_(texture_scene) {
      // Create the tile sprite
      tile_sprite = sprite_iamge;

      StoreTiles();
      CreateEntities();
    }
    
    void StoreTiles();
    void CreateEntities();
    
  private:
    Entity CreateBackgroundEntity(EnttScene* scene, char tile_type, uint32_t x, uint32_t y);
    
    uint32_t map_height_;
    EnttScene* tile_scene_, *texture_scene_;
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
   8 : Steps
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
   4 : Forest 4
   2 : Forest 2
   3 : Forest 3
   / -------------------------------------------------------------------------------------- */
  // For now making number of row even to make position of y integral
  // will move to level folder/class
  static std::string map_tile_pattern =
  "                                                                                                                                                                                                                                                                                                                                                                                                                               0"
  "                                                                                                                                                                                                                                                                                                                                                                                                                               0"
  "                                                                                                         (^)                                                                   (^)                                        (^)                                       (^)                                                                  (^)                                        (^)                                        0"
  "                                     (^)                    (^)                                                                   (^)                                                                                                                                                       (^)                                                                                                                    .           0"
  "                                                                             (^)                                                                   (^)                                                                                    (^)                                       (^)                      (^)                                                                                    (^)           ...          0"
  "                                                                                                                                                                                                       (^)                                       (^)                                                                                                             (^)                                              |u|          0"
  "                                                                                                 (^)                                                                   (^)                                                                                                                                                       (^)                                                                              |o|          0"
  "                                                                                                                                                                                                                    (^)                                       (^)                                                                                                             (^)                               .......        0"
  "                                                 B                  SXXS                             B            XBX                     SXXS                             B            XBX                                                                                                         SXXS                             B            XBX                                                           |u|r|u|        0"
  "                                                                                                                                                                                                                                                                                                                                                                                                                |o|||o|        0"
  "                                                                                                                                                                                                                                                                                                                                                                                                              ...........      0"
  "                                                                                                                                                                                                                                                                                                                                                                                                              |||||||||||      0"
  "                                                                                                                                                                                                                                                                                                                                                                                                              |l|u|r|u|l|      0"
  "                                               XBXBX              XBXXXXBX                           S           XXXXX                  XBXXXXBX                           S           XXXXX               B                                         B                                            XBXXXXBX                           S           XXXXX               B                                        |||o|||o|||      0"
  "                                                                                  Y                 SS                                                  Y                 SS                                                   S                                                                                  Y                 SS                                                   S                  ...............    0"
  "                                                *             Y                   !                SSS                              Y                   !                SSS                         *                        SS               *                                              Y                   !                SSS                         *                        SS                  |||||||||||||||    0"
  "                                               {4}            !                   !               SSSS                              !                   !               SSSS                        {4}    Y                 SSS              {4}    Y               *                        !                   !               SSSS                        {4}    Y                 SSS                  |u||u||u||u||u|    0"
  "                          <v>      <vv>       {213}     8     !     S  S          !     <vv>     SSSSS                        8     !     S  S          !     <vv>     SSSSS                       {213}   !                SSSS         <v> {213}   !              {1}            <v>  8     !     S  S          !     <vv>     SSSSS                       {213}   !                SSSS         <v>      |o||o||E||o||o|    0"
  "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG--------------------GGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG--------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG--------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
  "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG                    GGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG                    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG                    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
  ;
  
}

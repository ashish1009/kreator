//
//  background.cpp
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#include "background.hpp"

namespace mario {
  
#define USE_SPRITE 0
  
  /// This function returns the entity name from Char code
  /// - Parameter type: Char of tile map
  static std::string GetEntityNameFromChar(char type) {
    switch(type) {
      case 'G' : return "Ground";
        
      case '|' : return "Castel Brick";
      case 'o' : return "Castel Gate";
      case 'u' : return "castel Gate Domb";
      case '.' : return "Castel Domb";
      case 'l' : return "Castel Windlow Left";
      case 'r' : return "Castel Window Right";
        
      case 'S' : return "Steps";
      case '-' : return "Bridge";
      case '!' : return "Pipe Base";
      case 'Y' : return "Pipe";
      case 'X' : return "Bricks";
      case 'B' : return "Bonus";
      case 'b' : return "UsedBonus";
        
      case '<' : return "Grass <";
      case 'v' : return "Grass";
      case '>' : return "Grass >>";
        
      case '(' : return "Cloud Left";
      case '^' : return "Cloud";
      case ')' : return "Cloud Right";
        
      case '}' : return "Grass }";
      case '{' : return "Grass {";
      case '*' : return "Grass *";
      case '1' : return "Grass 1";
      case '2' : return "Grass 2";
      case '3' : return "Grass 3";
    };
    IK_ASSERT(false, "Invalid Type");
  }
  
  /// This function returns is char of tile is rigid
  /// - Parameter type: Char of tile map
  static bool IsRigid(char type) {
    switch(type) {
      case 'G' : return true; // "Ground";
        
      case '|' : return false; // "Castel Brick";
      case 'o' : return false; // "Castel Gate";
      case 'u' : return false; // "castel Gate Domb";
      case '.' : return false; // "Castel Domb";
      case 'l' : return false; // "Castel Windlow Left";
      case 'r' : return false; // "Castel Window Right";
        
      case 'S' : return true; // "Steps";
      case '-' : return true; // "Bridge";
      case '!' : return true; // "Pipe Base";
      case 'Y' : return true; // "Pipe";
      case 'X' : return true; // "Bricks";
      case 'B' : return true; // "Bonus";
      case 'b' : return true; // "UsedBonus";
        
      case '<' : return false; // "Grass <";
      case 'v' : return false; // "Grass v";
      case '>' : return false; // "Grass >>";
        
      case '(' : return false; // "Cloud Left";
      case '^' : return false; // "Cloud";
      case ')' : return false; // "Cloud Right";
        
      case '}' : return false; // "Grass }";
      case '{' : return false; // "Grass {";
      case '*' : return false; // "Grass *";
      case '1' : return false; // "Grass 1";
      case '2' : return false; // "Grass 2";
      case '3' : return false; // "Grass 3";
    };
    IK_ASSERT(false, "Invalid Type");
  }
  
  /// This function returns should it render subtexture or quad with texture
  /// - Parameter type: Char of tile map
  static bool IsSubtexture(char type) {
    switch(type) {
      case '|' : // "Castel Brick";
      case 'o' : // "Castel Gate";
      case 'u' : // "castel Gate Domb";
      case '.' : // "Castel Domb";
      case 'l' : // "Castel Windlow Left";
      case 'r' : // "Castel Window Right";
        
      case 'S' :// "Steps";
      case '-' :// "Bridge";
      case '!' :// "Pipe Base";
      case 'Y' :// "Pipe";
      case 'X' :// "Bricks";
      case 'B' :// "Bonus";
      case 'b' :// "UsedBonus";
        
      case '}' : // "Grass }";
      case '{' : // "Grass {";
      case '*' : // "Grass *";
      case '1' : // "Grass 1";
      case '2' : // "Grass 2";
      case '3' : // "Grass 3";
        return true;
#if USE_SPRITE
      case 'G' :// "Ground";

      case '(' : // Cloud Left
      case '^' : // Cloud
      case ')' : // Cloud Right

      case '<' : // "Grass <";
      case 'v' : // "Grass v";
      case '>' : // "Grass >>";
        return true;
#else
      case 'G' : // "Ground";

      case '(' : // Cloud Left
      case '^' : // Cloud
      case ')' : // Cloud Right

      case '<' : // "Grass <";
      case 'v' : // "Grass";
      case '>' : // "Grass >>";

        return false;
#endif
    };
    IK_ASSERT(false, "Invalid Type");
  }
  
  /// This function returns should a quad to render if no subtexture
  bool RenderQuad(char type) {
#if !USE_SPRITE
    switch(type) {
      case '(' : // Cloud Left
      case ')' : // Cloud Right
      case '<' : // "Grass <";
      case '>' : // "Grass >>";
        return false;
    };
#endif
    return true;
  }

  void BackgroudData::StoreTiles() {
    // Setup all the tile map
    tiles_char_map['G'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 27.0f });  // Ground
    
    tiles_char_map['X'] = SubTexture::CreateFromCoords(tile_sprite, { 17.0f, 27.0f }); // Bricks
    tiles_char_map['B'] = SubTexture::CreateFromCoords(tile_sprite, { 24.0f, 27.0f }); // Bonus
    tiles_char_map['b'] = SubTexture::CreateFromCoords(tile_sprite, { 27.0f, 27.0f }); // Used Bonus
    tiles_char_map['S'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 26.0f });  // Steps
    tiles_char_map['-'] = SubTexture::CreateFromCoords(tile_sprite, { 15.0f, 18.5f }); // Bridge
    tiles_char_map['Y'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 19.0f }, { 2.0f, 1.0f }); // Pipe Top
    tiles_char_map['!'] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 18.0f }, { 2.0f, 1.0f }); // Pipe base
    
    tiles_char_map['<'] = SubTexture::CreateFromCoords(tile_sprite, { 11.0f, 18.0f }); // Grass Left
    tiles_char_map['v'] = SubTexture::CreateFromCoords(tile_sprite, { 12.0f, 18.0f }); // Grass Mid
    tiles_char_map['>'] = SubTexture::CreateFromCoords(tile_sprite, { 13.0f, 18.0f }); // Grass Right
    
    tiles_char_map['{'] = SubTexture::CreateFromCoords(tile_sprite, { 8.0f, 19.0f });  // Forest Left
    tiles_char_map['}'] = SubTexture::CreateFromCoords(tile_sprite, { 10.0f, 19.0f }); // Forest Right
    tiles_char_map['*'] = SubTexture::CreateFromCoords(tile_sprite, { 9.0f, 19.0f });  // Forest Top
    tiles_char_map['1'] = SubTexture::CreateFromCoords(tile_sprite, { 8.0f, 18.0f });  // Forest Mid
    tiles_char_map['2'] = SubTexture::CreateFromCoords(tile_sprite, { 9.0f, 18.0f });  // Forest top left
    tiles_char_map['3'] = SubTexture::CreateFromCoords(tile_sprite, { 10.0f, 18.0f }); // Forest top Right
    
    tiles_char_map['('] = SubTexture::CreateFromCoords(tile_sprite, { 0.0f, 0.0f }, { 1.0f, 2.0f }); // Cloud Left
    tiles_char_map['^'] = SubTexture::CreateFromCoords(tile_sprite, { 1.0f, 0.0f }, { 1.0f, 2.0f }); // Cloud mid
    tiles_char_map[')'] = SubTexture::CreateFromCoords(tile_sprite, { 2.0f, 0.0f }, { 1.0f, 2.0f }); // Cloud Left
    
    tiles_char_map['.'] = SubTexture::CreateFromCoords(tile_sprite, { 19.0f, 27.0f }); // Castel Doomb
    tiles_char_map['u'] = SubTexture::CreateFromCoords(tile_sprite, { 20.0f, 26.0f }); // Castel Gate Domb
    tiles_char_map['o'] = SubTexture::CreateFromCoords(tile_sprite, { 21.0f, 26.0f }); // Castel gate
    tiles_char_map['|'] = SubTexture::CreateFromCoords(tile_sprite, { 21.0f, 27.0f }); // Castel Brick
    tiles_char_map['l'] = SubTexture::CreateFromCoords(tile_sprite, { 20.0f, 27.0f }); // Castel Window Left
    tiles_char_map['r'] = SubTexture::CreateFromCoords(tile_sprite, { 22.0f, 27.0f }); // Castel Window Right
    
    // Textures
    texture_char_map['^'] = Renderer::GetTexture(AM::ClientAsset("textures/background/cloud.png"));
    texture_char_map['v'] = Renderer::GetTexture(AM::ClientAsset("textures/background/grass.png"));
    texture_char_map['G'] = Renderer::GetTexture(AM::ClientAsset("textures/background/ground.jpeg"));
  }
  
  void BackgroudData::CreateEntities() {
    // ---------------------------------------------------------
    // Create the entity for each tile
    // ---------------------------------------------------------
    IK_INFO("Mario", "Creating Entity for each tile ");
    // Extract the map width. MAP Width should be same for each New line string
    size_t map_width = map_tile_pattern.find_first_of('0') + 1;
    uint32_t map_height = static_cast<uint32_t>(strlen(map_tile_pattern.c_str())) / map_width;
    
    for (uint32_t y = 0; y < map_height; y++) {
      for (uint32_t x = 0; x < map_width; x++) {
        // Create entity if we have sub texture for the character we found in map
        if (char tile_type = map_tile_pattern[x + y * map_width];
            tiles_char_map.find(tile_type) != tiles_char_map.end() or texture_char_map.find(tile_type) != texture_char_map.end()) {

          IK_INFO("Mario", " ---------------------------- ");
          auto entity = scene_->CreateEntity(GetEntityNameFromChar(tile_type));

          // Add rigid component
          if (IsRigid(tile_type)) {
            AABB aabb;
            aabb.min = { -0.5f, -0.5f, -0.5f };
            aabb.max = { 0.5f, 0.5f, 0.5f };
            entity.AddComponent<RigidBodyComponent>(aabb);
          }

          // Update the position
          auto& tc = entity.GetComponent<TransformComponent>();
          tc.translation = { (float)x - (float)30, (map_height / 2.0f) - y, 0.0f };
          
          if (IsSubtexture(tile_type)) {
            // Add sprite component
            const auto& sprite_comp = entity.AddComponent<SpriteComponent>(tiles_char_map[tile_type]);
            const auto& sprite_size = sprite_comp.sub_texture->GetSpriteSize();
            
            // Change scale acc to sprite
            tc.scale = { sprite_size.x, sprite_size.y , 0.0f};
          } else {
            if (RenderQuad(tile_type)) { // Some HACK for cloud and Grass
              auto& qc = entity.AddComponent<QuadComponent>();
              qc.texture_comp.use = true;
              qc.texture_comp.component = texture_char_map[tile_type];
                
              if (GetEntityNameFromChar(tile_type) == "Cloud" or GetEntityNameFromChar(tile_type) == "Grass" ) {
                entity.GetComponent<TransformComponent>().translation = { (float)x - (float)30, (map_height / 2.0f) - y + 0.5, 0.1f };
                entity.GetComponent<TransformComponent>().scale = { 4.0f, 2.0f, 1.0f };
              }
            } // if (RenderQuad(tile_type))
          } // if (IsSubtexture(tile_type))
        } // if (char tile_type = map_tile_pattern[x + y * map_width];
        else {
          if (tile_type != ' ' and tile_type != '0') // No need to validate Space
            IK_WARN("Mario", "    Char {0} at position Row {1} and Column {2} is not found in Tile Map", tile_type, y, x);
        }
      }
    }
  }
  
}

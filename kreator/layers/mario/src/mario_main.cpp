//
//  mario_main.cpp
//  mario
//
//  Created by Ashish . on 02/01/23.
//

#include "mario_main.hpp"

namespace mario {
  
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
  
  struct BackgroudData {
    // Sprite texture for background tiles
    std::shared_ptr<Texture> tile_sprite;

    // Map of subtexture to the Char (defined in s_MapTiles) of their corresponding tile.
    std::unordered_map<char, std::shared_ptr<SubTexture>> tiles_char_map;
    
    void Init();
  };
  static BackgroudData* background_data_;
  
  void BackgroudData::Init() {
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
  
  MarioLayer::MarioLayer() : Layer("Kreator") {
    IK_INFO("Mario", "Creating Mario Layer instance ... ");
    
    BatchRenderer::Reinit(1000, 0, 0);
    
    // Create memory for background data
    background_data_ = new BackgroudData();
    // Create the tile sprite
    background_data_->tile_sprite = Renderer::GetTexture(AM::ClientAsset("textures/tiles.png"));
    // Initialize the subtexture
    background_data_->Init();

    // Create the entity for each tile
    IK_INFO("Mario", "Creating Entity for each tile ");
    // Extract the map width. MAP Width should be same for each New line string
    size_t map_width = map_tile_pattern.find_first_of('0') + 1;
    uint32_t map_height = static_cast<uint32_t>(strlen(map_tile_pattern.c_str())) / map_width;

    for (uint32_t y = 0; y < map_height; y++) {
      for (uint32_t x = 0; x < map_width; x++) {
        // Create entity if we have sub texture for the character we found in map
        if (char tile_type = map_tile_pattern[x + y * map_width];
            background_data_->tiles_char_map.find(tile_type) != background_data_->tiles_char_map.end()) {
          auto entity = mario_scene_.CreateEntity(GetEntityNameFromChar(tile_type));
          const auto& sprite_comp = entity.AddComponent<SpriteComponent>(background_data_->tiles_char_map[tile_type]);
          const auto& sprite_size = sprite_comp.sub_texture->GetSpriteSize();
          
          auto& tc = entity.GetComponent<TransformComponent>();
          tc.translation = { x, (map_height / 2.0f) - y, 0.0f };
          tc.scale = { sprite_size.x, sprite_size.y , 0.0f};
        }
        else {
          if (tile_type != ' ' || tile_type != '0') // No need to validate Space
            IK_WARN("Mario", "    Char {0} at position Row {1} and Column {2} is not found in Tile Map", tile_type, y, x);
        }
      }
    }

  }
  
  MarioLayer::~MarioLayer() {
    IK_WARN("Mario", "Destroying Mario Layer instance !!! ");
    delete background_data_;
  }
  
  void MarioLayer::Attach() {
    IK_INFO("Mario", "Attaching Mario Layer instance");
    
    // Set the scene as playing
    mario_scene_.PlayScene();

    // Create the camera entity
    camera_entity_ = mario_scene_.CreateEntity();
    auto& camera_comp = camera_entity_.AddComponent<CameraComponent>();
    camera_comp.is_primary = true;
    camera_comp.camera->SetOrthographicSize(22.0f);

    camera_entity_.GetComponent<TransformComponent>().translation.y = 1.0f;
  }
  
  void MarioLayer::Detach() {
    IK_WARN("Mario", "Detaching Mario Layer instance ");
  }
  
  void MarioLayer::Update(Timestep ts) {
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
      mario_scene_.SetViewport(viewport_.width, viewport_.height);
    }
    
    viewport_.framebuffer->Bind();
    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    
    mario_scene_.Update(ts);

    viewport_.framebuffer->Unbind();
  }
  
  void MarioLayer::EventHandler(Event& event) {
  }
  
  void MarioLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();
    
    viewport_.RenderGui();
    mario_scene_.RenderGui();

    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Mario");
    ImGui::PushID("Mario");
    
    viewport_.focused = ImGui::IsWindowFocused();
    viewport_.hovered = ImGui::IsWindowHovered();
    
    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    viewport_.width = viewport_panel_size.x;
    viewport_.height = viewport_panel_size.y;
    
    size_t textureID = viewport_.framebuffer->GetColorAttachmentIds().at(0);
    ImGui::Image((void*)textureID,
                 viewport_panel_size,
                 ImVec2{ 0, 1 },
                 ImVec2{ 1, 0 });
    
    viewport_.UpdateBound();
        
    ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::End();

    ImguiAPI::EndDcocking();
  }
  
}

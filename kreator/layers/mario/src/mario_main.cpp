//
//  mario_main.cpp
//  mario
//
//  Created by Ashish . on 02/01/23.
//

#include "mario_main.hpp"
#include "camera_controller.h"

namespace mario {
  
  MarioLayer::MarioLayer() : Layer("Kreator") {
    IK_INFO("Mario", "Creating Mario Layer instance ... ");
    
    // Reinitialize the Batch Renderer
    BatchRenderer::Reinit(1000, 0, 0);
    
    // Create memory for background data
    background_data_ = new BackgroudData(Renderer::GetTexture(AM::ClientAsset("textures/tiles.png"), false));
  }
  
  MarioLayer::~MarioLayer() {
    IK_WARN("Mario", "Destroying Mario Layer instance !!! ");
    delete background_data_;
  }
  
  void MarioLayer::Attach() {
    IK_INFO("Mario", "Attaching Mario Layer instance");
    
    // ---------------------------------------------------------
    // Set the scene as playing
    // ---------------------------------------------------------
    mario_scene_.PlayScene();

    // ---------------------------------------------------------
    // Create the camera entity
    // ---------------------------------------------------------
    camera_entity_ = mario_scene_.CreateEntity();
    camera_entity_.GetComponent<TransformComponent>().translation.y = 2.0f;

    auto& camera_comp = camera_entity_.AddComponent<CameraComponent>();
    camera_comp.is_primary = true;
    camera_comp.camera->SetOrthographicSize(22.0f);

    camera_entity_.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    
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
            background_data_->tiles_char_map.find(tile_type) != background_data_->tiles_char_map.end()) {
          auto entity = mario_scene_.CreateEntity(GetEntityNameFromChar(tile_type));
          const auto& sprite_comp = entity.AddComponent<SpriteComponent>(background_data_->tiles_char_map[tile_type]);
          const auto& sprite_size = sprite_comp.sub_texture->GetSpriteSize();
          
          auto& tc = entity.GetComponent<TransformComponent>();
          tc.translation = { x, (map_height / 2.0f) - y, 0.0f };
          tc.scale = { sprite_size.x, sprite_size.y , 0.0f};
        }
        else {
          if (tile_type != ' ' and tile_type != '0') // No need to validate Space
            IK_WARN("Mario", "    Char {0} at position Row {1} and Column {2} is not found in Tile Map", tile_type, y, x);
        }
      }
    }
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

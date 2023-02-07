//
//  mario_layer.cpp
//  kreator
//
//  Created by Ashish . on 07/02/23.
//

#include "mario_layer.hpp"

namespace mario {
  
  RendererLayer::RendererLayer() : Layer("Mario") {
    IK_INFO("Mario", "Creating Mario Layer instance ... ");
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN("Mario", "Destroying Mario Layer instance !!! ");
  }
    
  void RendererLayer::Attach() {
    IK_INFO("Mario", "Attaching Mario Layer instance");
  }
  
  void RendererLayer::Detach() {
    IK_WARN("Mario", "Detaching Mario Layer instance ");
  }
    
  void RendererLayer::Update(Timestep ts) {
    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
  }
  
  void RendererLayer::EventHandler(Event& event) {
  }
  
  void RendererLayer::RenderGui() {
    if (!settings_.play) {
      Renderer::Framerate();
      Renderer::RenderStatsGui();
      viewport_.RenderGui();
      
      // Texture for Play and Pause button
      static std::shared_ptr<Texture> play_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/play.png"));
      
      // Play Pause Buttom
      ImGui::Begin("Play/Pause",
                   nullptr, // No Open Flag
                   ImGuiWindowFlags_NoDecoration |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse);
            
      float size = ImGui::GetWindowHeight() - 12.0f; // 12 just random number
      ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
      
      // Button action
      if (PropertyGrid::ImageButton("Play/Pause", play_texture->GetRendererID(), { size, size })) {
        settings_.play = true;
      }
      ImGui::End();
    }
  }
  
}

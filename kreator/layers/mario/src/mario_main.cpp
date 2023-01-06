//
//  mario_main.cpp
//  mario
//
//  Created by Ashish . on 02/01/23.
//

#include "mario_main.hpp"

namespace mario {
  
  MarioLayer::MarioLayer() : Layer("Kreator") {
    IK_INFO("Mario", "Creating Mario Layer instance ... ");
  }
  
  MarioLayer::~MarioLayer() {
    IK_WARN("Mario", "Destroying Mario Layer instance !!! ");
  }
  
  void MarioLayer::Attach() {
    IK_INFO("Mario", "Attaching Mario Layer instance");
    
    mario_scene_.PlayScene();
    
    tile_sprite_ = Renderer::GetTexture(AM::ClientAsset("textures/tiles.png"));
    
    auto e = mario_scene_.CreateEntity();
    e.AddComponent<QuadComponent>();
    
    auto c = mario_scene_.CreateEntity();
    c.AddComponent<CameraComponent>().is_primary = true;

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
    Renderer::RenderStatsGui(nullptr, true);
    
    viewport_.RenderGui();

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

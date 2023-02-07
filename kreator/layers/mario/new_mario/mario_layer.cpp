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
    active_scene_ = std::make_shared<EnttScene>();
  }
  
  void RendererLayer::Detach() {
    IK_WARN("Mario", "Detaching Mario Layer instance ");
  }
    
  void RendererLayer::Update(Timestep ts) {
    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    active_scene_->Update(ts);
  }
  
  void RendererLayer::EventHandler(Event& event) {
    active_scene_->EventHandler(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(RendererLayer::KeyPressed));
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(RendererLayer::WindowResized));
  }
  
  void RendererLayer::RenderGui() {
    if (settings_.play) {
    }
    else {
      active_scene_->RenderGui();

      if (active_scene_->IsEditing()) {
        Renderer::Framerate(nullptr);
        Renderer::RenderStatsGui(nullptr, true);
        
        viewport_.RenderGui();
        MarioPlayButton();
      }
    }
  }
  
  bool RendererLayer::KeyPressed(KeyPressedEvent& event) {
    if (event.GetKeyCode() == KeyCode::Escape) {
      settings_.play = false;
    }
    return false;
  }
  
  bool RendererLayer::WindowResized(WindowResizeEvent& event) {
    active_scene_->SetViewport(event.GetWidth(), event.GetHeight());
    return false;
  }
  
  void RendererLayer::MarioPlayButton() {
    static std::shared_ptr<Texture> play_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/play.png"));
    ImGui::Begin("Play/Pause", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    float size = ImGui::GetWindowHeight() - 12.0f; // 12 just random number
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    
    // Button action
    if (PropertyGrid::ImageButton("Play/Pause", play_texture->GetRendererID(), { size, size })) {
      settings_.play = true;
    }
    ImGui::End();
  }
  
  void ScenePlayPauseButton() {
    
  }
}

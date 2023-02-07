//
//  mario_layer.cpp
//  kreator
//
//  Created by Ashish . on 07/02/23.
//

#include "mario_layer.hpp"

namespace ikan_game {
  
#define is_playing settings_.play
  
  // Game Controller
  static const std::string game_name_ = "Mario";
  
  // Imgui Font
  static const std::string current_font_path_ = "fonts/mario.ttf";
  static const std::string current_bold_font_path_ = "fonts/mario.ttf";
  
  RendererLayer::RendererLayer() : Layer(game_name_) {
    IK_INFO(game_name_, "Creating {0} Layer instance ... ", game_name_.c_str());
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN(game_name_, "Destroying {0} Layer instance !!! ", game_name_.c_str());
  }
    
  void RendererLayer::Attach() {
    IK_INFO(game_name_, "Attaching {0} Layer instance", game_name_.c_str());
    
    cbp_.AddFavouritPaths({
      AM::ProjectPath("kreator/layers/mario/assets"),
      AM::ProjectPath("kreator/layers/mario/assets/scenes"),
    });

    // Decorate the Imgui Change the font of imgui
    ImguiAPI::ChangeFont(
                         // Regular Font information
                         { AM::ClientAsset(current_font_path_), 14.0f /* Size of font */ },
                         // Bold Font information
                         { AM::ClientAsset(current_bold_font_path_), 14.0f /* Size of font */ }
                         );
    
    ImguiAPI::SetLightGreyThemeColors();

    active_scene_ = std::make_shared<EnttScene>();
  }
  
  void RendererLayer::Detach() {
    IK_WARN(game_name_, "Detaching {0} Layer instance ", game_name_.c_str());
  }
    
  void RendererLayer::Update(Timestep ts) {
    if (is_playing) {
      RenderScene(ts);
    }
    else {
      viewport_.framebuffer->Bind();
      RenderScene(ts);
      viewport_.framebuffer->Unbind();
    }
  }
  
  void RendererLayer::EventHandler(Event& event) {
    active_scene_->EventHandler(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(RendererLayer::KeyPressed));
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(RendererLayer::WindowResized));
  }
  
  void RendererLayer::RenderGui() {
    if (is_playing) {
    }
    else {
      ImguiAPI::StartDcocking();
      
      active_scene_->RenderGui();

      Renderer::Framerate(nullptr);
      Renderer::RenderStatsGui(nullptr, true);
      
      viewport_.RenderGui();
      GamePlayButton();
      ScenePlayPauseButton();
      cbp_.RenderGui();

      RenderViewport();
      
      ImguiAPI::EndDcocking();
    }
  }
  
  bool RendererLayer::KeyPressed(KeyPressedEvent& event) {
    if (event.GetKeyCode() == KeyCode::Escape) {
      is_playing = false;
    }
    return false;
  }
  
  bool RendererLayer::WindowResized(WindowResizeEvent& event) {
    active_scene_->SetViewport(event.GetWidth(), event.GetHeight());
    return false;
  }
  
  void RendererLayer::RenderScene(Timestep ts) {
    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    active_scene_->Update(ts);

  }
  
  void RendererLayer::GamePlayButton() {
    static std::shared_ptr<Texture> play_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/play.png"));
    ImGui::Begin("Game Play", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    float size = ImGui::GetWindowHeight() - 12.0f; // 12 just random number
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    
    // Button action
    if (PropertyGrid::ImageButton("Play/Pause", play_texture->GetRendererID(), { size, size })) {
      is_playing = true;
    }
    PropertyGrid::HoveredMsg("Play Button for Game");
    ImGui::End();
  }
  
  void RendererLayer::ScenePlayPauseButton() {
    // Texture for Play and Pause button
    static std::shared_ptr<Texture> pause_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/pause.png"));
    static std::shared_ptr<Texture> play_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/play.png"));
    
    // Play Pause Buttom
    ImGui::Begin("Scene Play/Pause", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PushID("Scene Play/pause");
    
    // Update the texture id based on the state of scene
    uint32_t tex_id = active_scene_->IsEditing() ? play_texture->GetRendererID() : pause_texture->GetRendererID();
    
    float size = ImGui::GetWindowHeight() - 12.0f; // 12 just random number
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    
    // Button action
    if (PropertyGrid::ImageButton("Play/Pause", tex_id, { size, size })) {
      if (active_scene_->IsEditing())
        active_scene_->PlayScene();
      else
        active_scene_->EditScene();
    }
    PropertyGrid::HoveredMsg("Play Button for Scene (Debug Scene in play mode)");
    ImGui::PopID();
    ImGui::End();
  }
  
  void RendererLayer::RenderViewport() {
    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Kreator Viewport");
    ImGui::PushID("Kreator Viewport");
    
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
  }
  
}

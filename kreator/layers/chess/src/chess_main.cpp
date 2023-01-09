//
//  chess_main.cpp
//  chess
//
//  Created by Ashish . on 02/01/23.
//

#include "chess_main.hpp"

namespace chess {
  
  ChessLayer::ChessLayer() : Layer("Kreator") {
    IK_INFO("Chess", "Creating Chess Layer instance ... ");
    
    // Reinitialize the Batch Renderer
    BatchRenderer::Reinit(100, 0, 0);
    
#if CHESS_DEBUG
    spm_.SetSceneContext(&chess_scene_);
#endif
  }
  
  ChessLayer::~ChessLayer() {
    IK_WARN("Chess", "Destroying Chess Layer instance !!! ");
  }
  
  void ChessLayer::Attach() {
    IK_INFO("Chess", "Attaching Chess Layer instance");
    // ---------------------------------------------------------
    // Set the scene as playing
    // ---------------------------------------------------------
    chess_scene_.PlayScene();

    // ---------------------------------------------------------
    // Create the camera entity
    // ---------------------------------------------------------
    camera_entity_ = chess_scene_.CreateEntity();
    camera_entity_.GetComponent<TransformComponent>().translation.y = 2.0f;
    
    auto& camera_comp = camera_entity_.AddComponent<CameraComponent>();
    camera_comp.is_primary = true;
    camera_comp.camera->SetOrthographicSize(22.0f);

  }
  
  void ChessLayer::Detach() {
    IK_WARN("Chess", "Detaching Chess Layer instance ");
  }
  
  void ChessLayer::Update(Timestep ts) {
#if CHESS_DEBUG
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
      // TODO: Store the player position before resize and back it up after resize
      chess_scene_.SetViewport(viewport_.width, viewport_.height);
    }
    
    viewport_.UpdateMousePos();
    viewport_.framebuffer->Bind();
    
    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    chess_scene_.Update(ts);
    
    viewport_.UpdateHoveredEntity(&spm_);
    viewport_.framebuffer->Unbind();
#else
    Renderer::Clear({0.2, 0.2, 0.2, 1.0});
    chess_scene_.Update(ts);
#endif
  }
  
  void ChessLayer::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(ChessLayer::OnWindowResized));
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(ChessLayer::OnMouseButtonPressed));
  }
  
  bool ChessLayer::OnWindowResized(WindowResizeEvent &e) {
    viewport_width_ = e.GetWidth();
    viewport_height_ = e.GetHeight();
    
    // TODO: Store the player position before resize and back it up after resize
    chess_scene_.SetViewport(viewport_width_, viewport_height_);
    return false;
  }
  
  bool ChessLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
#if CHESS_DEBUG
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      if (viewport_.mouse_pos_x >= 0 and
          viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and
          viewport_.mouse_pos_y <= viewport_.height) {
        spm_.SetSelectedEntity(viewport_.hovered_entity_);
      }
    }
#endif
    return false;
  }

  
  void ChessLayer::RenderGui() {
#if CHESS_DEBUG
    ImguiAPI::StartDcocking();
    Renderer::RenderStatsGui();
    Renderer::Framerate();
    
    viewport_.RenderGui();
    chess_scene_.RenderGui();
        
    spm_.RenderGui();
    
    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Chess");
    ImGui::PushID("Chess");
    
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
#endif
  }

}

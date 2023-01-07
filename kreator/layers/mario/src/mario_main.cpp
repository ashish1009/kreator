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
    
#if MARIO_DEBUG
    spm_.SetSceneContext(&mario_scene_);
#endif
  }
  
  MarioLayer::~MarioLayer() {
    IK_WARN("Mario", "Destroying Mario Layer instance !!! ");
  }
  
  void MarioLayer::Attach() {
    IK_INFO("Mario", "Attaching Mario Layer instance");
    
    // ---------------------------------------------------------
    // Set the scene as playing
    // ---------------------------------------------------------
    mario_scene_.PlayScene();
    
    // ---------------------------------------------------------
    // Create memory for background data
    // ---------------------------------------------------------
    background_data_ = new BackgroudData(&mario_scene_,
                                         Renderer::GetTexture(AM::ClientAsset("textures/tiles.png"), false));

    // ---------------------------------------------------------
    // Create the camera entity
    // ---------------------------------------------------------
    camera_entity_ = mario_scene_.CreateEntity();
    camera_entity_.GetComponent<TransformComponent>().translation.y = 2.0f;

    auto& camera_comp = camera_entity_.AddComponent<CameraComponent>();
    camera_comp.is_primary = true;
    camera_comp.camera->SetOrthographicSize(22.0f);

    camera_entity_.AddComponent<NativeScriptComponent>([](NativeScriptComponent* sc,
                                                          const std::string& script_name) {
      if (script_name == "mario::CameraController") {
        sc->Bind<mario::CameraController>();
        return true;
      }
      return false;
    }).Bind<CameraController>();
    
    // --------------------------------------------------------
    // Player
    // --------------------------------------------------------
    player_ = new Player(&mario_scene_);
  }
  
  void MarioLayer::Detach() {
    IK_WARN("Mario", "Detaching Mario Layer instance ");
    delete background_data_;
    delete player_;
  }
  
  void MarioLayer::Update(Timestep ts) {
#if MARIO_DEBUG
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
      // TODO: Store the player position before resize and back it up after resize
      mario_scene_.SetViewport(viewport_.width, viewport_.height);
    }

    viewport_.UpdateMousePos();
    viewport_.framebuffer->Bind();

    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    mario_scene_.Update(ts);

    viewport_.UpdateHoveredEntity(&spm_);
    viewport_.framebuffer->Unbind();
#else
    Renderer::Clear({0.2, 0.3, 0.4, 1.0});
    mario_scene_.Update(ts);
#endif
  }
  
  void MarioLayer::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(MarioLayer::OnWindowResized));
#if MARIO_DEBUG
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(MarioLayer::OnMouseButtonPressed));
#endif
  }
  
  bool MarioLayer::OnWindowResized(WindowResizeEvent &e) {
    viewport_width_ = e.GetWidth();
    viewport_height_ = e.GetHeight();
    
#if !MARIO_DEBUG
    // TODO: Store the player position before resize and back it up after resize
    mario_scene_.SetViewport(viewport_width_, viewport_height_);
#endif
    return false;
  }
  
#if MARIO_DEBUG
  bool MarioLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      if (viewport_.mouse_pos_x >= 0 and
          viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and
          viewport_.mouse_pos_y <= viewport_.height) {
        spm_.SetSelectedEntity(viewport_.hovered_entity_);
      }
    }
    return false;
  }
#endif
  
  void MarioLayer::RenderGui() {
#if MARIO_DEBUG
    
    ImguiAPI::StartDcocking();
    Renderer::RenderStatsGui();
    Renderer::Framerate();

    viewport_.RenderGui();
    mario_scene_.RenderGui();
    
    ImGui::Begin("Setting");
    ImGui::PushID("Setting");
    
    if (ImGui::Button("Reset")) {
      player_->Reset();
      camera_entity_.GetComponent<TransformComponent>().translation.x = 0.0f;
    }
    
    ImGui::PopID();
    ImGui::End();
    
    spm_.RenderGui();
    player_->RenderGui();

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
#endif
  }
  
}

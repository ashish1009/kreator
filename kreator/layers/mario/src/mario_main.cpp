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

    camera_entity_.AddComponent<NativeScriptComponent>().Bind<CameraController>();
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

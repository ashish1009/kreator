//
//  editor_layer.cpp
//  kreator
//
//  Created by Ashish . on 28/12/22.
//

#include "editor_layer.hpp"

namespace editor {
  
  EditorLayer::EditorLayer() : Layer("Kreator") {
    IK_INFO("Creating Editor Layer instance ... ");
  }
  
  EditorLayer::~EditorLayer() {
    IK_WARN("Destroying Editor Layer instance !!! ");
  }
  
  void EditorLayer::Attach() {
    IK_INFO("Attaching Editor Layer instance");
  }
  
  void EditorLayer::Detach() {
    IK_WARN("Detaching Editor Layer instance ");
  }
  
  void EditorLayer::Update(Timestep ts) {
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
    }
    Renderer::ResetStatsEachFrame();
    
    viewport_.framebuffer->Bind();

    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    viewport_.UpdateMousePos();

    viewport_.framebuffer->Unbind();
  }
  
  void EditorLayer::EventHandler(Event& event) {
  }
    
  void EditorLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();
    
    viewport_.RenderGui();
    
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
    
    ImguiAPI::EndDcocking();
  }
  
} // namespace sandbox

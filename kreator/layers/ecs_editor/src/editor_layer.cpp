//
//  editor_layer.cpp
//  ecs_editor
//
//  Created by Ashish . on 28/12/22.
//

#include "editor_layer.hpp"

namespace editor {
  
  EditorLayer::EditorLayer() : Layer("Kreator") {
    IK_INFO("Editor", "Creating Editor Layer instance ... ");
    cbp_.AddFavouritPaths({
      AM::ProjectPath("kreator/layers/ecs_editor/editor_assets"),
    });
  }
  
  EditorLayer::~EditorLayer() {
    IK_WARN("Editor", "Destroying Editor Layer instance !!! ");
  }
  
  void EditorLayer::Attach() {
    IK_INFO("Editor", "Attaching Editor Layer instance");
  }
  
  void EditorLayer::Detach() {
    IK_WARN("Editor", "Detaching Editor Layer instance ");
  }
  
  void EditorLayer::Update(Timestep ts) {
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
      active_scene_.SetViewport(viewport_.width, viewport_.height);
    }
    
    Renderer::ResetStatsEachFrame();
    
    viewport_.framebuffer->Bind();

    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    active_scene_.Update(ts);
    
    viewport_.UpdateMousePos();
    viewport_.framebuffer->Unbind();
  }
  
  void EditorLayer::EventHandler(Event& event) {
    active_scene_.EventHandler(event);
  }
    
  void EditorLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui(true);
    
    viewport_.RenderGui();
    active_scene_.RenderGui();
    cbp_.RenderGui();
    
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
  
} 

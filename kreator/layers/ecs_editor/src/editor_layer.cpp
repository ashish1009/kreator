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
    
    spm_.SetSceneContext(&active_scene_);
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
    viewport_.UpdateMousePos();
    
    viewport_.framebuffer->Bind();

    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    active_scene_.Update(ts);
    
    UpdateHoveredEntity();
    viewport_.framebuffer->Unbind();
  }
  
  void EditorLayer::EventHandler(Event& event) {
    active_scene_.EventHandler(event);
  }
    
  void EditorLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate(&setting_.frame_rate);
    Renderer::RenderStatsGui(&setting_.stats, true);
    
    viewport_.RenderGui(&setting_.viewport);
    cbp_.RenderGui(&setting_.cbp);
    spm_.RenderGui(&setting_.spm);

    active_scene_.RenderGui();
    
    // Show Menu bar
    ShowMenu();

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
    
  void EditorLayer::ShowMenu() {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit", "Cmd + Q")) {
          Application::Get().Close();
        }
        ImGui::EndMenu(); // ImGui::BeginMenu("File")
      } // if (ImGui::BeginMenu("File"))
      
      if (ImGui::BeginMenu("Setting")) {
        if (ImGui::BeginMenu("Scene")) {
          Setting::UpdateSetting("Editor Camera", active_scene_.GetSetting().editor_camera);
          ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
        }
        ImGui::Separator();

        Setting::UpdateSetting("Content Browser Panel", setting_.cbp);
        Setting::UpdateSetting("Scene Panel", setting_.spm);
        ImGui::Separator();

        Setting::UpdateSetting("Frame Rate", setting_.frame_rate);
        Setting::UpdateSetting("Viewpoer", setting_.viewport);
        Setting::UpdateSetting("Renderer Stats", setting_.stats);

        ImGui::EndMenu(); // ImGui::BeginMenu("Setting")
      }

      ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    } // if (ImGui::BeginMenuBar())
  }

  void EditorLayer::Setting::UpdateSetting(std::string tag, bool& flag) {
    if (ImGui::MenuItem(tag.c_str(), nullptr, flag))
      flag = (flag) ? false : true;
  }

  void EditorLayer::UpdateHoveredEntity() {
    if (!viewport_.hovered)
      return;
    
    // Get pixel from rednerer
    Renderer::GetEntityIdFromPixels(viewport_.mouse_pos_x,
                                    viewport_.mouse_pos_y,
                                    1,
                                    viewport_.hovered_entity_id_);
    // Update hovered entity
    viewport_.hovered_entity_ = (viewport_.hovered_entity_id_ > (int32_t)active_scene_.GetMaxEntityId()) ?
    nullptr :
    active_scene_.GetEnitityFromId(viewport_.hovered_entity_id_);
  }

} 

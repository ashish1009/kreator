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
    
    viewport_.UpdateMousePos();
    
    viewport_.framebuffer->Bind();

    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    active_scene_.Update(ts);
    
    viewport_.UpdateHoveredEntity(&spm_);
    viewport_.framebuffer->Unbind();
  }
  
  void EditorLayer::EventHandler(Event& event) {
    active_scene_.EventHandler(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
  }

  bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      if (viewport_.mouse_pos_x >= 0 and
          viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and
          viewport_.mouse_pos_y <= viewport_.height and
          viewport_.hovered_entity_) {
        spm_.SetSelectedEntity(viewport_.hovered_entity_);
      }
    }
    
    return false;
  }
  
  bool EditorLayer::OnKeyPressed(KeyPressedEvent& event) {
    if (event.GetRepeatCount() > 0)
      return false;
    
    // Set control key
    bool ctrl = Input::IsKeyPressed(KeyCode::LeftControl) or Input::IsKeyPressed(KeyCode::RightControl);
    
    switch (event.GetKeyCode()) {
        // -------------------------------
        // Gizmos
        // -------------------------------
      case KeyCode::Q:
        if (ctrl)
          viewport_.guizmo_type = -1;
        break;
      case KeyCode::W:
        if (ctrl)
          viewport_.guizmo_type = ImGuizmo::OPERATION::TRANSLATE;
        break;
      case KeyCode::E:
        if (ctrl)
          viewport_.guizmo_type = ImGuizmo::OPERATION::ROTATE;
        break;
      case KeyCode::R:
        if (ctrl)
          viewport_.guizmo_type = ImGuizmo::OPERATION::SCALE;
        
      default:
        break;
    }
    return false;
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

    OnImguizmoUpdate();

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

  void EditorLayer::OnImguizmoUpdate() {
    Entity* selected_entity = spm_.GetSelectedEntity();
    if (selected_entity and viewport_.guizmo_type != -1) {
      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist();
      
      float window_width = (float)ImGui::GetWindowWidth();
      float window_height = (float)ImGui::GetWindowHeight();
      ImGuizmo::SetRect(ImGui::GetWindowPos().x,
                        ImGui::GetWindowPos().y,
                        window_width,
                        window_height);
      
      // Camera
      EditorCamera* editor_camera = active_scene_.GetEditorCamera();
      
      const glm::mat4& camera_projection = editor_camera->GetProjection();
      const glm::mat4& camera_view = editor_camera->GetView();
      
      // Entity transform
      auto& tc = selected_entity->GetComponent<TransformComponent>();
      glm::mat4 transform = tc.GetTransform();
      
      // Snapping
      bool snap = Input::IsKeyPressed(KeyCode::LeftControl);
      float snap_value = 0.5f; // Snap to 0.5m for translation/scale
      
      // Snap to 45 degrees for rotation
      if (viewport_.guizmo_type == ImGuizmo::OPERATION::ROTATE)
        snap_value = 45.0f;
      
      float snap_values[3] = {
        snap_value,
        snap_value,
        snap_value
      };
      
      ImGuizmo::Manipulate(glm::value_ptr(camera_view),
                           glm::value_ptr(camera_projection),
                           (ImGuizmo::OPERATION)viewport_.guizmo_type,
                           ImGuizmo::LOCAL,
                           glm::value_ptr(transform),
                           nullptr,
                           snap ? snap_values : nullptr);
      
      if (ImGuizmo::IsUsing()) {
        glm::vec3 translation, rotation, scale;
        Math::DecomposeTransform(transform, translation, rotation, scale);
        
        glm::vec3 deltaRotation = rotation - tc.rotation;
        tc.translation = translation;
        tc.rotation += deltaRotation;
        tc.scale = scale;
      }
    }
  }
} 

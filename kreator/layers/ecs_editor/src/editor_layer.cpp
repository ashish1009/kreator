//
//  editor_layer.cpp
//  ecs_editor
//
//  Created by Ashish . on 28/12/22.
//

#include "editor_layer.hpp"

namespace editor {
  
  /// This structure stores the data for rendering text like frame rate and renderer versions
  struct SystemTextData {
    // Projection matrix for still camera to make text not move with camera
    glm::mat4 still_camera_projection;
    
    // Render the Frame rate position
    glm::vec3 frame_rate_text_pos = { 5.0f, 5.0f, 0.3f };
    // Render the Renderer version position
    glm::vec3 renderer_info_text_pos;
    
    // Text size and color
    glm::vec2 text_size = { 0.25f, 0.25f };
    glm::vec4 text_color = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    uint32_t viewport_width = 1600;
    uint32_t viewport_height = 900;
    
    void UpdateData(uint32_t width, uint32_t height) {
      if (viewport_width == width and viewport_height == height)
        return;
      
      viewport_width = width;
      viewport_height = height;
      
      still_camera_projection = glm::ortho( 0.0f, (float)viewport_width, 0.0f, (float)viewport_height);
      renderer_info_text_pos = { viewport_width - 220.0f, 1.0f, 0.3f };
    }
    
    static void Render(uint32_t viewport_width, uint32_t viewport_height) {
      static SystemTextData text_rednerer_data;
      
      /// Render the Frame rate
      TextRenderer::RenderText(std::to_string((uint32_t)(ImGui::GetIO().Framerate)),
                               text_rednerer_data.still_camera_projection,
                               text_rednerer_data.frame_rate_text_pos,
                               text_rednerer_data.text_size,
                               text_rednerer_data.text_color);
      
      /// Render the Renderer Version
      static const Renderer::Capabilities& rendererCapability = Renderer::Capabilities::Get();
      static std::string rendererInfo = "(c) Kreator | "
      + rendererCapability.renderer + " | "
      + rendererCapability.version;
      TextRenderer::RenderText(rendererInfo,
                               text_rednerer_data.still_camera_projection,
                               text_rednerer_data.renderer_info_text_pos,
                               text_rednerer_data.text_size,
                               text_rednerer_data.text_color);
      
      text_rednerer_data.UpdateData(viewport_width, viewport_height);
    }
  };
  
  EditorLayer::EditorLayer() : Layer("Kreator") {
    IK_INFO("Editor", "Creating Editor Layer instance ... ");
    cbp_.AddFavouritPaths({
      AM::ProjectPath("kreator/layers/ecs_editor/editor_assets"),
      AM::ProjectPath("kreator/layers/ecs_editor/editor_assets/scenes"),
    });
  }
  
  EditorLayer::~EditorLayer() {
    IK_WARN("Editor", "Destroying Editor Layer instance !!! ");
  }
  
  void EditorLayer::Attach() {
    IK_INFO("Editor", "Attaching Editor Layer instance");
    
    // Change Text renderer Font
    TextRenderer::LoadFreetype(AM::ClientAsset("fonts/opensans/OpenSans-Regular.ttf"));
  }
  
  void EditorLayer::Detach() {
    IK_WARN("Editor", "Detaching Editor Layer instance ");
  }
  
  void EditorLayer::Update(Timestep ts) {
    if (!active_scene_)
      return;
    
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
      active_scene_->SetViewport(viewport_.width, viewport_.height);
    }
    
    viewport_.UpdateMousePos();
    
    viewport_.framebuffer->Bind();

    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    active_scene_->Update(ts);
    
    SystemTextData::Render(viewport_.width, viewport_.height);
        
    viewport_.UpdateHoveredEntity(&spm_);
    viewport_.framebuffer->Unbind();
  }
  
  void EditorLayer::EventHandler(Event& event) {
    if (active_scene_)
      active_scene_->EventHandler(event);

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
    
    // Set command key
    bool cmd = Input::IsKeyPressed(KeyCode::LeftSuper) or Input::IsKeyPressed(KeyCode::RightSuper);
    // Set control key
    bool ctrl = Input::IsKeyPressed(KeyCode::LeftControl) or Input::IsKeyPressed(KeyCode::RightControl);
    
    switch (event.GetKeyCode()) {
      case KeyCode::N:
        if (cmd)
          NewScene();
        break;
      case KeyCode::X:
        if (cmd)
          CloseScene();
        break;
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

    // Show Menu bar
    ShowMenu();
    
    if (!active_scene_) {
      ImguiAPI::EndDcocking();
      Renderer::Framerate(&setting_.frame_rate);
      Renderer::RenderStatsGui(&setting_.stats, true);
      viewport_.RenderGui(&setting_.viewport);
      cbp_.RenderGui(&setting_.cbp);
      
      return;
    }

    if (active_scene_->IsEditing()) {
      Renderer::Framerate(&setting_.frame_rate);
      Renderer::RenderStatsGui(&setting_.stats, true);
      
      SaveScene();
      
      viewport_.RenderGui(&setting_.viewport);
      spm_.RenderGui();
      cbp_.RenderGui(&setting_.cbp);
    }
    active_scene_->RenderGui();
    
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
    
    PropertyGrid::DropConent([this](const std::string& path)
                             {
      if (StringUtils::GetExtensionFromFilePath(path) == "ikanScene")
        OpenScene(path);
      else
        IK_WARN("Invalid file for Scene {0}", path.c_str());
    });
    
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
        if (ImGui::BeginMenu("Scene")) {
          if (ImGui::MenuItem("New", "Cmd + N"))
            NewScene();
          if (ImGui::MenuItem("Close", "Cmd + X"))
            CloseScene();
          
          ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
        } // if (ImGui::BeginMenu("Scene"))
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Cmd + Q")) {
          Application::Get().Close();
        }
        ImGui::EndMenu(); // ImGui::BeginMenu("File")
      } // if (ImGui::BeginMenu("File"))
      
      if (ImGui::BeginMenu("Setting", active_scene_ and active_scene_->IsEditing())) {
        if (ImGui::BeginMenu("Scene")) {
          Setting::UpdateSetting("Editor Camera", active_scene_->GetSetting().editor_camera);
          Setting::UpdateSetting("Scene Controller", active_scene_->GetSetting().scene_controller);
          ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
        }
        if (ImGui::BeginMenu("Scene Panels")) {
          Setting::UpdateSetting("Entity Panel", spm_.GetSetting().scene_panel);
          Setting::UpdateSetting("Property Panel", spm_.GetSetting().property_panel);
          ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene Panel"))
        }
        ImGui::Separator();
        // -------------------------------------------------------------------

        Setting::UpdateSetting("Save Scene Widget", setting_.save_scene);
        ImGui::Separator();
        // -------------------------------------------------------------------
        
        Setting::UpdateSetting("Content Browser Panel", setting_.cbp);
        ImGui::Separator();
        // -------------------------------------------------------------------
        
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
      EditorCamera* editor_camera = active_scene_->GetEditorCamera();
      
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
  
  void EditorLayer::CloseScene() {
    if (!active_scene_)
      return;

    active_scene_.reset();
    active_scene_ = nullptr;
  }

  const void EditorLayer::NewScene(const std::string& scene_path) {
    // Close the current scene
    CloseScene();
    
    // Create New Scene
    active_scene_ = std::make_shared<EnttScene>(scene_path);
    spm_.SetSceneContext(active_scene_.get());

    active_scene_->SetViewport((uint32_t)viewport_.width, (uint32_t)viewport_.height);
  }
  
  const void EditorLayer::OpenScene(const std::string& path) {
    IK_INFO("Editor", "Opening saved scene from {0}", path.c_str());
    
    NewScene(path);
    
    SceneSerializer serializer(active_scene_.get());
    serializer.Deserialize(path);
    
    active_scene_->SetViewport((uint32_t)viewport_.width, (uint32_t)viewport_.height);
  }
  
  const void EditorLayer::SaveScene() {
    if (!setting_ .save_scene)
      return;
    ImGui::Begin("Save File", &setting_.save_scene);
    ImGui::PushID("Save File");

    const auto& relative_path = (std::filesystem::relative(cbp_.GetCurrentDir(), cbp_.GetRootDir())).string();
    PropertyGrid::ReadOnlyTextBox("Scene Directory",
                                  relative_path,
                                  "File will be saved at the Current directory in the active scene",
                                  150.0f);
    
    static std::string file_name = "";
    bool modified = PropertyGrid::TextBox(file_name,
                                          "Scene Name",
                                          2,
                                          150.0f);
    
    if (modified) {
      std::string file_path = cbp_.GetCurrentDir().string() + "/" + file_name + ".ikanScene";
      
      IK_INFO("Editor", "Saving Scene at {0}", file_path.c_str());
      if (!file_path.empty()) {
        active_scene_->SetFilePath(file_path);
        SceneSerializer serializer(active_scene_.get());
        serializer.Serialize(file_path);
      }
    }

    ImGui::PopID();
    ImGui::End();
  }

} 

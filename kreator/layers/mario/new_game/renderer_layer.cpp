//
//  mario_layer.cpp
//  kreator
//
//  Created by Ashish . on 07/02/23.
//

#include "renderer_layer.hpp"
#include "mario/mario.hpp"

namespace ikan_game {

#define is_playing settings_.play
  
  RendererLayer::RendererLayer() : Layer("ikan Game") {
    game_data_ = std::make_unique<mario::MarioData>();
    
    IK_INFO(game_data_->GameName(), "Creating {0} Layer instance ... ", game_data_->GameName().c_str());
    
    // Mario Init
    {
      // Reinitialize the Batch Renderer
      BatchRenderer::Init(2000, 0, 0);
    }
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN(game_data_->GameName(), "Destroying {0} Layer instance !!! ", game_data_->GameName().c_str());
  }
    
  void RendererLayer::Attach() {
    IK_INFO(game_data_->GameName(), "Attaching {0} Layer instance", game_data_->GameName().c_str());
    
    cbp_.AddFavouritPaths(game_data_->FavDirecotries());

    // Decorate the Imgui Change the font of imgui
    ImguiAPI::ChangeFont(game_data_->RegularFontData(), game_data_->BoldFontData());
    
    // TODO: Add File Menu
    ImguiAPI::SetLightGreyThemeColors();

    // TODO: Openging File in start. Later will not do this
//    if (!OpenScene(AM::ClientAsset("scenes/Mario_Scene.ikanScene"))) {
//      NewScene(AM::ClientAsset("scenes/New_scene"));
//    }
  }
  
  void RendererLayer::Detach() {
    IK_WARN(game_data_->GameName(), "Detaching {0} Layer instance ", game_data_->GameName().c_str());
  }
    
  void RendererLayer::Update(Timestep ts) {
    if (!active_scene_)
      return;
    
    viewport_.UpdateMousePos();

    if (is_playing) {      
      RenderScene(ts);
    }
    else {
      if (viewport_.IsFramebufferResized()) {
        viewport_width = viewport_.width;
        viewport_height = viewport_.height;
        
        viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
        active_scene_->SetViewport(viewport_width, viewport_height);
      }

      viewport_.framebuffer->Bind();
      RenderScene(ts);

      RenderGrid();

      viewport_.UpdateHoveredEntity(spm_.GetSelectedEntity(), active_scene_.get());
      viewport_.framebuffer->Unbind();
    }
  }
  
  void RendererLayer::EventHandler(Event& event) {
    if (active_scene_)
      active_scene_->EventHandler(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(RendererLayer::KeyPressed));
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(RendererLayer::WindowResized));
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(RendererLayer::MouseButtonPressed));
  }
  
  void RendererLayer::RenderGui() {
    if (is_playing) {
    }
    else {
      ImguiAPI::StartDcocking();
      
      GamePlayButton();

      if (active_scene_) {
        active_scene_->RenderGui();
        
        if (active_scene_->IsEditing()) {
          Renderer::Framerate(nullptr);
          Renderer::RenderStatsGui(nullptr, true);
          viewport_.RenderGui();
          
          cbp_.RenderGui();
          spm_.RenderGui();
          
          SaveScene();
        }
        ScenePlayPauseButton();
        RenderViewport();
      }

      ImguiAPI::EndDcocking();
    }
  }
  
  bool RendererLayer::KeyPressed(KeyPressedEvent& event) {
    if (event.GetRepeatCount() > 0)
      return false;

    // Set control key
    bool ctrl = Input::IsKeyPressed(KeyCode::LeftControl) or Input::IsKeyPressed(KeyCode::RightControl);
    // Set command key
    bool cmd = Input::IsKeyPressed(KeyCode::LeftSuper) or Input::IsKeyPressed(KeyCode::RightSuper);

    switch (event.GetKeyCode()) {
        // File Manager
      case KeyCode::N:
        if (cmd)
          NewScene();
        break;
      case KeyCode::X:
        if (cmd)
          CloseScene();
        break;

        // Guizmo
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
        
        // Game Play controller
      case KeyCode::Escape:
        is_playing = false;

      default:
        break;
    }
    return false;
  }
  
  bool RendererLayer::WindowResized(WindowResizeEvent& event) {
    viewport_width = event.GetWidth();
    viewport_height = event.GetHeight();
    
    active_scene_->SetViewport(viewport_width, viewport_height);
    return false;
  }
  
  bool RendererLayer::MouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      if (viewport_.mouse_pos_x >= 0 and viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and viewport_.mouse_pos_y <= viewport_.height and
          viewport_.hovered_entity_) {
        spm_.SetSelectedEntity(viewport_.hovered_entity_);
      }
    }
    
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
    if (PropertyGrid::ImageButton("Game Play", play_texture->GetRendererID(), { size, size })) {
      is_playing = true;
      Application::Get().MaximizeWindow();
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
    if (PropertyGrid::ImageButton("Scene Play/Pause", tex_id, { size, size })) {
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
    ImGui::Image((void*)textureID, viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    if (active_scene_->IsEditing()) {
      PropertyGrid::DropConent([this](const std::string& path)
                               {
        if (StringUtils::GetExtensionFromFilePath(path) == "ikanScene")
          OpenScene(path);
        else
          IK_WARN("Invalid file for Scene {0}", path.c_str());
      });
      
      OnImguizmoUpdate();
    }
    
    viewport_.UpdateBound();
    
    ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::End();
  }

  const void RendererLayer::SaveScene() {
    ImGui::Begin("Save File");
    ImGui::PushID("Save File");
    
    const auto& relative_path = (std::filesystem::relative(cbp_.GetCurrentDir(), cbp_.GetRootDir())).string();
    PropertyGrid::ReadOnlyTextBox("Scene Directory", relative_path, "File will be saved at the Current directory in the active scene", 150.0f);
    
    static std::string file_name = "";
    bool modified = PropertyGrid::TextBox(file_name, "Scene Name", 2, 150.0f);
    
    if (modified) {
      std::string file_path = cbp_.GetCurrentDir().string() + "/" + file_name + ".ikanScene";
      
      IK_INFO(game_data_->GameName(), "Saving Scene at {0}", file_path.c_str());
      if (!file_path.empty()) {
        active_scene_->SetFilePath(file_path);
        SceneSerializer serializer(active_scene_.get());
        serializer.Serialize(file_path);
      }
    }
    
    ImGui::PopID();
    ImGui::End();
  }
  
  void RendererLayer::CloseScene() {
    if (!active_scene_)
      return;

    IK_INFO(game_data_->GameName(), "Closing Scene {0}", active_scene_->GetName().c_str());
    active_scene_.reset();
    active_scene_ = nullptr;
  }
  
  const void RendererLayer::NewScene(const std::string& scene_path) {
    // Close the current scene
    CloseScene();
    
    // Create New Scene
    IK_INFO(game_data_->GameName(), "Creating New Scene {0}", scene_path.c_str());
    active_scene_ = std::make_shared<EnttScene>(scene_path);
    spm_.SetSceneContext(active_scene_.get());    
  }

  const bool RendererLayer::OpenScene(const std::string& scene_path) {
    IK_INFO(game_data_->GameName(), "Opening saved scene from {0}", scene_path.c_str());
    
    NewScene(scene_path);
    SceneSerializer serializer(active_scene_.get());
    
    bool result = serializer.Deserialize(scene_path);
    active_scene_->SetViewport(viewport_width, viewport_height);
    
    return result;
  }
  
  void RendererLayer::OnImguizmoUpdate() {
    Entity* selected_entity = spm_.GetSelectedEntity();
    if (selected_entity and viewport_.guizmo_type != -1) {
      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist();
      
      float window_width = (float)ImGui::GetWindowWidth();
      float window_height = (float)ImGui::GetWindowHeight();
      ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);
      
      // Entity transform
      auto& tc = selected_entity->GetComponent<TransformComponent>();
      glm::mat4 transform = tc.GetTransform();
      
      // Snapping
      bool snap = Input::IsKeyPressed(KeyCode::LeftControl);
      float snap_value = 0.5f; // Snap to 0.5m for translation/scale
      
      // Snap to 45 degrees for rotation
      if (viewport_.guizmo_type == ImGuizmo::OPERATION::ROTATE)
        snap_value = 45.0f;
      
      float snap_values[3] = { snap_value, snap_value, snap_value };
      
      if (!active_scene_->UseEditorCamera()) {
        // Camera
        const glm::mat4& camera_projection = active_scene_->GetPrimaryCameraData().scene_camera->GetProjection();
        const glm::mat4& camera_view = glm::inverse(active_scene_->GetPrimaryCameraData().transform_comp->GetTransform());
        
        ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection), (ImGuizmo::OPERATION)viewport_.guizmo_type,
                             ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snap_values : nullptr);
        
      } else {
        // Camera
        EditorCamera* editor_camera = active_scene_->GetEditorCamera();
        
        const glm::mat4& camera_projection = editor_camera->GetProjection();
        const glm::mat4& camera_view = editor_camera->GetView();
        
        ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection), (ImGuizmo::OPERATION)viewport_.guizmo_type,
                             ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snap_values : nullptr);
      }
      
      if (ImGuizmo::IsUsing()) {
        glm::vec3 translation, rotation, scale;
        Math::DecomposeTransform(transform, translation, rotation, scale);
        
        glm::vec3 deltaRotation = rotation - tc.Rotation();
        tc.UpdateTranslation(translation);
        tc.UpdateRotation(tc.Rotation() + deltaRotation);
        tc.UpdateScale(scale);
      } // if (ImGuizmo::IsUsing())
    } // if (selected_entity and viewport_.guizmo_type != -1)
  }
  
  void RendererLayer::RenderGrid() {
    const auto& cd = active_scene_->GetPrimaryCameraData();
    if (cd.scene_camera)
      cd.scene_camera->RenderGrids(300, {0.6, 0.6, 0.6, 1.0}, cd.transform_matrix);
  }
}

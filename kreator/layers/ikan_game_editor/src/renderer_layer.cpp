//
//  renderer_layer.cpp
//  kreator
//
//  Created by Ashish . on 07/02/23.
//

#include "renderer_layer.hpp"

namespace ikan_game {

#define is_playing settings_.play
  
  RendererLayer::RendererLayer(GameType type) : Layer("ikan Game"), game_data_(CreateGameData(type, &viewport_)) {
    cbp_.SetRootData(game_data_->CbpRootDir());
    IK_INFO(game_data_->GameName(), "Creating {0} Layer instance ... ", game_data_->GameName().c_str());
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN(game_data_->GameName(), "Destroying {0} Layer instance !!! ", game_data_->GameName().c_str());
  }
    
  void RendererLayer::Attach() {
    IK_INFO(game_data_->GameName(), "Attaching {0} Layer instance", game_data_->GameName().c_str());
    
    cbp_.AddFavouritPaths(game_data_->FavDirecotries());
    
    BatchRenderer::Init(100, 1000, 10000);

    // Decorate the Imgui Change the font of imgui
    ImguiAPI::ChangeFont(game_data_->RegularFontData(), game_data_->BoldFontData());
    ImguiAPI::SetLightGreyThemeColors();

    if (!OpenScene(game_data_->OpenSavedScene()))
      NewScene(AM::ClientAsset("scenes/New_scene"));
  }
  
  void RendererLayer::Detach() {
    IK_WARN(game_data_->GameName(), "Detaching {0} Layer instance ", game_data_->GameName().c_str());
  }
    
  void RendererLayer::Update(Timestep ts) {
    if (!active_scene_)
      return;

    viewport_.UpdateMousePos();

    if (is_playing) {
      Renderer::Clear(game_data_->GetBgColor());
      RenderScene(ts);
    }
    else {
      if (viewport_.IsFramebufferResized()) {
        viewport_width = viewport_.width;
        viewport_height = viewport_.height;

        viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
        active_scene_->SetViewport(viewport_width, viewport_height);

        game_data_->SetViewportSize(viewport_width, viewport_height);
      }

      viewport_.framebuffer->Bind();
      Renderer::Clear(viewport_.framebuffer->GetSpecification().color);

      RenderScene(ts);
      
      if (!game_data_->IsPlaying()) {
        SelectEntities();
        MoveCameraDebug(ts);
      }

      if (settings_.show_grids)
        RenderGrid();

      if (settings_.show_colliders)
        OverlayRender();

      viewport_.UpdateHoveredEntity(spm_.GetSelectedEntity(), active_scene_.get());
      viewport_.framebuffer->Unbind();
    }
  }
  
  void RendererLayer::EventHandler(Event& event) {
    if (active_scene_)
      active_scene_->EventHandler(event);

    game_data_->EventHandler(event);

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

      ShowMenu();
      GamePlayButton();

      if (active_scene_) {
        ShowSettings();

        active_scene_->RenderGui();

        Renderer::Framerate(&settings_.frame_rate);
        Renderer::RenderStatsGui(&settings_.stats, true);
        Renderer::Render2DStatsGui(&settings_.stats_2d);
        viewport_.RenderGui(&settings_.viewport);

        cbp_.RenderGui(&settings_.cbp);
        spm_.RenderGui();
        game_data_->RenderGui();

        if (active_scene_->IsEditing()) {
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

    bool ctrl  = Input::IsKeyPressed(KeyCode::LeftControl) or Input::IsKeyPressed(KeyCode::RightControl);
    bool cmd   = Input::IsKeyPressed(KeyCode::LeftSuper) or Input::IsKeyPressed(KeyCode::RightSuper);
    bool shift = Input::IsKeyPressed(KeyCode::LeftShift);

    if (cmd) {
      switch (event.GetKeyCode()) {
        case KeyCode::T: PlayScene();   break;
        case KeyCode::R: SetPlay(true); break;
          
        // File Manager
        case KeyCode::N: NewScene();   break;
        case KeyCode::X: CloseScene(); break;
          
        default: break;
      };
    }
    
    if (ctrl) {
      switch (event.GetKeyCode()) {
        case KeyCode::Q: viewport_.guizmo_type = -1;                              break;
        case KeyCode::W: viewport_.guizmo_type = ImGuizmo::OPERATION::TRANSLATE;  break;
        case KeyCode::E: viewport_.guizmo_type = ImGuizmo::OPERATION::ROTATE;     break;
        case KeyCode::R: viewport_.guizmo_type = ImGuizmo::OPERATION::SCALE;      break;
        default: break;
      }
    }
    
    if (shift) {
      switch (event.GetKeyCode()) {
        case KeyCode::D:          DuplicateSelectedEntities();  break;
        case KeyCode::Backspace:  DeleteSelectedEntities();     break;
        case KeyCode::Escape:     ClearSelectedEntities();      break;
          
        case KeyCode::Left:     MoveEntities(Left);   break;
        case KeyCode::Right:    MoveEntities(Right);  break;
        case KeyCode::Up:       MoveEntities(Up);     break;
        case KeyCode::Down:     MoveEntities(Down);   break;
          
        default: break;
      } // switch (e.GetKeyCode())
    } // if (shift)

    switch (event.GetKeyCode()) {
      case KeyCode::Escape: SetPlay(false); break;
      default: break;
    }
    
    return false;
  }
  
  bool RendererLayer::WindowResized(WindowResizeEvent& event) {
    viewport_width = event.GetWidth();
    viewport_height = event.GetHeight();
    
    active_scene_->SetViewport(viewport_width, viewport_height);
    game_data_->SetViewportSize(viewport_width, viewport_height);
    return false;
  }
  
  bool RendererLayer::MouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      if (viewport_.mouse_pos_x >= 0 and viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and viewport_.mouse_pos_y <= viewport_.height) {
          spm_.SetSelectedEntity(viewport_.hovered_entity_);
      }
    }
    return false;
  }

  void RendererLayer::RenderScene(Timestep ts) {
    active_scene_->Update(ts);
    game_data_->Update(ts);
  }
  
  void RendererLayer::GamePlayButton() {
    static std::shared_ptr<Texture> play_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/play.png"));
    ImGui::Begin("Game Play", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    float size = ImGui::GetWindowHeight() - 12.0f; // 12 just random number
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    
    // Button action
    if (PropertyGrid::ImageButton("Game Play", play_texture->GetRendererID(), { size, size })) {
      SetPlay(true);
      PlayScene();
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
      if (active_scene_->IsEditing()) {
        PlayScene();
      }
      else {
        active_scene_ = editor_scene_;
        active_scene_->EditScene();

        game_data_->Init(active_scene_, &spm_);
        spm_.SetSceneContext(active_scene_.get());
      }
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
    if (!settings_ .save_scene)
      return;

    ImGui::Begin("Save File", &settings_.save_scene);
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
    
    editor_scene_.reset();
    editor_scene_ = nullptr;
  }
  
  const void RendererLayer::NewScene(const std::string& scene_path) {
    // Close the current scene
    CloseScene();
    
    // Create New Scene
    IK_INFO(game_data_->GameName(), "Creating New Scene {0}", scene_path.c_str());
    editor_scene_ = std::make_shared<EnttScene>(scene_path);
    
    active_scene_ = editor_scene_;
    spm_.SetSceneContext(active_scene_.get());
  }

  const bool RendererLayer::OpenScene(const std::string& scene_path) {
    IK_INFO(game_data_->GameName(), "Opening saved scene from {0}", scene_path.c_str());
    
    // Close the current scene
    CloseScene();
    
    editor_scene_ = std::make_shared<EnttScene>(scene_path);
    editor_scene_->SetViewport(viewport_width, viewport_height);
    
    SceneSerializer serializer(editor_scene_.get());
    
    bool result = serializer.Deserialize(scene_path);
        
    active_scene_ = editor_scene_;
    spm_.SetSceneContext(active_scene_.get());
    game_data_->Init(active_scene_, &spm_);

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
        const auto& cd = active_scene_->GetPrimaryCameraData();
        const glm::mat4& camera_projection = cd.scene_camera->GetProjection();

        if (cd.scene_camera->GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
          const glm::mat4& camera_view = glm::inverse(Math::GetTransformMatrix({cd.position.x, cd.position.y, cd.position.z + 1.0f},
                                                                               cd.transform_comp->Rotation(),
                                                                               cd.transform_comp->Scale()));
          ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection), (ImGuizmo::OPERATION)viewport_.guizmo_type,
                               ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snap_values : nullptr);
        }
        else {
        const glm::mat4& camera_view = glm::inverse(cd.transform_comp->GetTransform());
          ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection), (ImGuizmo::OPERATION)viewport_.guizmo_type,
                               ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snap_values : nullptr);
        }
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
      cd.scene_camera->RenderGrids(300, {0.6, 0.6, 0.6, 0.2}, cd.transform_matrix, cd.position);
  }
  
  void RendererLayer::ShowMenu() {
    if (ImGui::BeginMenuBar()) {
      // -------------------------------------------------------------
      //  File Menu
      // -------------------------------------------------------------
      if (ImGui::BeginMenu("File")) {
        if (ImGui::BeginMenu("Scene")) {
          if (ImGui::MenuItem("New", "Cmd + N"))    NewScene();
          if (ImGui::MenuItem("Close", "Cmd + X"))  CloseScene();
          
          ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
        } // if (ImGui::BeginMenu("Scene"))
        // -------------------------------
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Cmd + Q")) {
          Application::Get().Close();
        }
        ImGui::EndMenu(); // ImGui::BeginMenu("File")
      } // if (ImGui::BeginMenu("File"))
      
      // -------------------------------------------------------------
      //  Property Menu
      // -------------------------------------------------------------
      if (ImGui::BeginMenu("Properties")) {
        if (ImGui::BeginMenu("Theme")) {
          if (ImGui::MenuItem("Light", nullptr)) {
            viewport_.framebuffer->UpdateSpecificationColor({0.82f, 0.82f, 0.82f, 1.0f});
            ImguiAPI::SetLightThemeColors();
            current_theme_ = Theme::Light;
          }
          if (ImGui::MenuItem("Dark", nullptr)) {
            viewport_.framebuffer->UpdateSpecificationColor({0.08f, 0.08f, 0.08f, 1.0f});
            ImguiAPI::SetDarkThemeColors();
            current_theme_ = Theme::Dark;
          }
          if (ImGui::MenuItem("Grey", nullptr)) {
            viewport_.framebuffer->UpdateSpecificationColor({0.18f, 0.18f, 0.18f, 1.0f});
            ImguiAPI::SetGreyThemeColors();
            current_theme_ = Theme::Grey;
          }
          if (ImGui::MenuItem("Light Grey", nullptr)) {
            viewport_.framebuffer->UpdateSpecificationColor({0.25f, 0.25f, 0.25f, 1.0f});
            ImguiAPI::SetLightGreyThemeColors();
            current_theme_ = Theme::LightGrey;
          }
          ImGui::EndMenu(); // ImGui::BeginMenu("Theme")
        } // if (ImGui::BeginMenu("Theme"))
        ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
      } // if (ImGui::BeginMenu("Properties"))
      
      // -------------------------------------------------------------
      //  Setting Menu
      // -------------------------------------------------------------
      if (ImGui::BeginMenu("Setting", active_scene_ and active_scene_->IsEditing())) {
        if (ImGui::BeginMenu("Scene")) {
          Setting::UpdateSetting("Editor Camera", active_scene_->GetSetting().editor_camera);
          Setting::UpdateSetting("Show Colliders", settings_.show_colliders);
          Setting::UpdateSetting("Show Grids", settings_.show_grids);
          ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
        }
        if (ImGui::BeginMenu("Scene Panels")) {
          Setting::UpdateSetting("Entity Panel", spm_.GetSetting().scene_panel);
          Setting::UpdateSetting("Property Panel", spm_.GetSetting().property_panel);
          ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene Panel"))
        }
        ImGui::Separator();
        // -------------------------------------------------------------------

        Setting::UpdateSetting("Save Scene Widget", settings_.save_scene);
        ImGui::Separator();
        // -------------------------------------------------------------------

        Setting::UpdateSetting("Content Browser Panel", settings_.cbp);
        ImGui::Separator();
        // -------------------------------------------------------------------

        Setting::UpdateSetting("Frame Rate", settings_.frame_rate);
        Setting::UpdateSetting("Viewport", settings_.viewport);
        Setting::UpdateSetting("Renderer Stats", settings_.stats);

        ImGui::EndMenu(); // ImGui::BeginMenu("Setting")
      }
      ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    } // if (ImGui::BeginMenuBar())
  }

  void RendererLayer::SetPlay(bool is_play) {
    is_playing = is_play;
    game_data_->SetState(is_play);
    Application::Get().MaximizeWindow();
  }
  
  void RendererLayer::OverlayRender() {
    static const glm::vec4 collider_color = {0.0f, 1.0f, 0.0f, 1.0f};
    bool camera_found = false;
  
    if (active_scene_->UseEditorCamera()) {
      BatchRenderer::BeginBatch(active_scene_->GetEditorCamera()->GetViewProjection());
      camera_found = true;
    }
    else {
      const auto& cd = active_scene_->GetPrimaryCameraData();
      if (cd.scene_camera)
        camera_found = true;
      
      if (!camera_found)
        return;
      
      BatchRenderer::BeginBatch(cd.scene_camera->GetProjection() * glm::inverse(cd.transform_matrix));
    }
    // Box coilider
    {
      auto view = active_scene_->GetEntitesWith<TransformComponent, BoxColliderComponent>();
      for (auto entity : view) {
        auto [tc, bcc] = view.get<TransformComponent, BoxColliderComponent>(entity);
        
        glm::vec3 p = tc.Translation() + glm::vec3(bcc.offset, 0.001f);
        glm::vec3 s = tc.Scale() * glm::vec3((bcc.size * 2.0f), 1.0f); // We need diameter
        
        BatchRenderer::DrawRect(Math::GetTransformMatrix(p, tc.Rotation(), s), collider_color);
      }
    }

    // Circle coilider
    {
      auto view = active_scene_->GetEntitesWith<TransformComponent, CircleColliiderComponent>();
      for (auto entity : view) {
        auto [tc, ccc] = view.get<TransformComponent, CircleColliiderComponent>(entity);
        
        glm::vec3 p = tc.Translation() + glm::vec3(ccc.offset, 0.001f);
        glm::vec3 s = tc.Scale() * glm::vec3(ccc.radius * 2.0f); // We need diameter
        
        // Rotation ???
        
        BatchRenderer::DrawCircle(Math::GetTransformMatrix(p, {0, 0, 0}, s), collider_color, 0.05f);
      }
    }
    
    // Pill box coilider
    {
      auto view = active_scene_->GetEntitesWith<TransformComponent, PillBoxColliderComponent>();
      for (auto entity : view) {
        auto [tc, pbc] = view.get<TransformComponent, PillBoxColliderComponent>(entity);
        pbc.RecalculateColliders();
        {
          const auto& bcc = pbc.bcc;

          glm::vec3 p = tc.Translation() + glm::vec3(bcc.offset, 0.001f);
          glm::vec3 s = glm::vec3((bcc.size * 2.0f), 1.0f); // We need diameter

          BatchRenderer::DrawRect(Math::GetTransformMatrix(p, tc.Rotation(), s), collider_color);
        }
        {
          const auto& ccc = pbc.top_ccc;
          glm::vec3 p = tc.Translation() + glm::vec3(ccc.offset, 0.001f);
          glm::vec3 s = glm::vec3(ccc.radius * 2.0f); // We need diameter
          
          // Rotation ???
          BatchRenderer::DrawCircle(Math::GetTransformMatrix(p, {0, 0, 0}, s), collider_color, 0.05f);
        }
        {
          const auto& ccc = pbc.bottom_ccc;
          glm::vec3 p = tc.Translation() + glm::vec3(ccc.offset, 0.001f);
          glm::vec3 s = glm::vec3(ccc.radius * 2.0f); // We need diameter
          
          // Rotation ???
          BatchRenderer::DrawCircle(Math::GetTransformMatrix(p, {0, 0, 0}, s), collider_color, 0.05f);
        }
      }
    }
    BatchRenderer::EndBatch();
  }
  
  void RendererLayer::PlayScene() {
    active_scene_ = EnttScene::Copy(editor_scene_);
    spm_.SetSceneContext(active_scene_.get());
    
    game_data_->Init(active_scene_, &spm_);
    
    // After Set contact listner
    active_scene_->PlayScene();
  }

  void RendererLayer::ShowSettings() {
    ImGui::Begin("Settings");

    PropertyGrid::CheckBox("Use Editor Camera", active_scene_->GetSetting().use_editor_camera);
    if (active_scene_->GetSetting().use_editor_camera)
      PropertyGrid::CheckBox("Show Editor Camera", active_scene_->GetSetting().editor_camera);
    PropertyGrid::CheckBox("Show Colliders", settings_.show_colliders);
    PropertyGrid::CheckBox("Scene Debug Draw", active_scene_->GetSetting().debug_draw_);
    PropertyGrid::CheckBox("Show Grids", settings_.show_grids);

    ImGui::Separator();
    PropertyGrid::CheckBox("Entity Panel", spm_.GetSetting().scene_panel);
    PropertyGrid::CheckBox("Property Panel", spm_.GetSetting().property_panel);

    ImGui::Separator();
    PropertyGrid::CheckBox("Content Browser Panel", settings_.cbp);

    ImGui::Separator();
    PropertyGrid::CheckBox("Save Scene", settings_.save_scene);
    PropertyGrid::CheckBox("Show Colliders", settings_.show_colliders);
    
    ImGui::Separator();
    PropertyGrid::CheckBox("Frame Rate", settings_.frame_rate);
    PropertyGrid::CheckBox("Viewport Data", settings_.viewport);
    PropertyGrid::CheckBox("Renderer Stats", settings_.stats);

    ImGui::End();
  }
  
  void RendererLayer::SelectEntities() {
    if (!(viewport_.mouse_pos_x >= 0 and viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and viewport_.mouse_pos_y <= viewport_.height)) {
      ClearSelectedEntities();
      return;
    }
    
    if (!Input::IsKeyPressed(KeyCode::LeftShift) or Input::IsKeyPressed(KeyCode::LeftControl))
      return;
    
    static bool first_clicked = true;
    
    static glm::vec2 initial_mouse_position_ = glm::vec2(0.0f);
    static glm::vec2 final_mouse_position_ = glm::vec2(0.0f);
    
    static glm::vec2 initial_block_position_ = glm::vec2(0.0f);
    static glm::vec2 final_block_position_ = glm::vec2(0.0f);
    
    const auto& cd = active_scene_->GetPrimaryCameraData();
    float zoom = cd.scene_camera->GetZoom();
    float aspect_ratio = cd.scene_camera->GetAspectRatio();

    if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft)) {
      if (first_clicked) {
        ClearSelectedEntities();
        
        first_clicked = false;
        initial_mouse_position_ = { viewport_.mouse_pos_x, viewport_.mouse_pos_y };
        initial_block_position_ = {
          viewport_.mouse_pos_x - ((float)viewport_.width / 2),
          viewport_.mouse_pos_y - ((float)viewport_.height / 2)
        };
        initial_block_position_ *= ((zoom * aspect_ratio) / viewport_.width);
      }
      
      final_mouse_position_ = { viewport_.mouse_pos_x, viewport_.mouse_pos_y };
      final_block_position_ = {
        viewport_.mouse_pos_x - ((float)viewport_.width / 2),
        viewport_.mouse_pos_y - ((float)viewport_.height / 2)
      };
      final_block_position_ *= ((zoom * aspect_ratio) / viewport_.width);
      
      // Render the outline rectangle
      BatchRenderer::BeginBatch(active_scene_->GetPrimaryCameraData().scene_camera->GetProjection() *
                                glm::inverse(active_scene_->GetPrimaryCameraData().transform_matrix));
      BatchRenderer::DrawRect({initial_block_position_.x + cd.position.x, initial_block_position_.y + cd.position.y, 0.1},
                              {final_block_position_.x + cd.position.x, final_block_position_.y + cd.position.y, 0.1},
                              {1, 1, 1, 1});
      BatchRenderer::EndBatch();
      
    }
    if (Input::IsMouseButtonReleased(MouseButton::ButtonLeft)) {
      if (!first_clicked) {
        // Store entites present in selected entitity
        float min_x = std::min(initial_mouse_position_.x, final_mouse_position_.x);
        float max_x = std::max(initial_mouse_position_.x, final_mouse_position_.x);
        float min_y = std::min(initial_mouse_position_.y, final_mouse_position_.y);
        float max_y = std::max(initial_mouse_position_.y, final_mouse_position_.y);
        
        for (float i_x = min_x; i_x <= max_x; i_x ++) {
          for (float i_y = min_y; i_y <= max_y; i_y++) {
            // Get pixel from rednerer
            int32_t pixel = -1;
            
            Renderer::GetEntityIdFromPixels(i_x, i_y, viewport_.framebuffer->GetPixelIdIndex(), pixel);
#if 0
            IK_TRACE(game_data_->GameName(), "X : {0}, Y : {1}, Pixel : {2}", i_x, i_y, pixel);
#endif
            
            if (pixel <= (int32_t)active_scene_->GetMaxEntityId()) {
              if (selected_entities_.find((entt::entity)pixel) == selected_entities_.end()){
                selected_entities_[(entt::entity)pixel] = active_scene_->GetEnitityFromId(pixel);
              }
            }
          }
        }
        
        // TODO: Do it while adding to vector? or keep separate???
        HighlightSelectedEntities(true);
      }
      first_clicked = true;
    }
  }
  
  void RendererLayer::MoveEntities(Direction direction) {
    for (auto& [entt, entity] : selected_entities_) {
      if(!entity) continue;
      
      auto& tc = entity->GetComponent<TransformComponent>();
      switch (direction) {
        case Down:      tc.AddTranslation_Y(- 1.0f);     break;
        case Up:        tc.AddTranslation_Y(1.0f);       break;
        case Right:     tc.AddTranslation_X(1.0f);       break;
        case Left:      tc.AddTranslation_X(- 1.0f);     break;
        default: break;
      }
    }
  }
  
  void RendererLayer::DeleteSelectedEntities() {
    for (auto& [entt, entity] : selected_entities_) {
      if (spm_.GetSelectedEntity() and *(spm_.GetSelectedEntity()) == *entity) {
        spm_.SetSelectedEntity(nullptr);
      }
      active_scene_->DestroyEntity(*entity);
    }
    selected_entities_.clear();
  }
  
  void RendererLayer::DuplicateSelectedEntities() {
    HighlightSelectedEntities(false);
    for (auto& [entt, entity] : selected_entities_) {
      active_scene_->DuplicateEntity(*entity);
    }
    HighlightSelectedEntities(true);
  }
  
  void RendererLayer::ClearSelectedEntities() {
    HighlightSelectedEntities(false);
    selected_entities_.clear();
  }
  
  void RendererLayer::HighlightSelectedEntities(bool enable) {
    for (auto& [entt, entity] : selected_entities_) {
      if(!entity) continue;
      
      auto& qc = entity->GetComponent<QuadComponent>();
      if (enable)
        qc.color.a -=0.2f;
      else
        qc.color.a +=0.2f;
    }
  }
  
  void RendererLayer::MoveCameraDebug(Timestep ts) {
    // Move Camera for debug
    auto& cd = active_scene_->GetPrimaryCameraData();
    if (cd.scene_camera) {
      auto& cam = cd.scene_camera;
      auto& tc = cd.transform_comp;
      
      bool shift = Input::IsKeyPressed(KeyCode::RightShift);
      if (shift) {
        if (Input::IsKeyPressed(KeyCode::A)) tc->AddTranslation_X(-(cam->GetZoom() * ts));
        if (Input::IsKeyPressed(KeyCode::D)) tc->AddTranslation_X(cam->GetZoom() * ts);
        
        if (Input::IsKeyPressed(KeyCode::W)) tc->AddTranslation_Y(cam->GetZoom() * ts);
        if (Input::IsKeyPressed(KeyCode::S)) tc->AddTranslation_Y(-(cam->GetZoom() * ts));
        
        if (Input::IsKeyPressed(KeyCode::Q)) cam->SetOrthographicSize(cam->GetOrthographicSize() + 1.0f);
        if (Input::IsKeyPressed(KeyCode::E)) cam->SetOrthographicSize(cam->GetOrthographicSize() - 1.0f);
      }
    }
  }

}

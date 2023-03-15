//
//  renderer_layer.cpp
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#include "renderer_layer.hpp"

namespace kreator {
  
  RendererLayer::RendererLayer(GameType game_type) : Layer("Kreator"), game_data_(CreateGameData(game_type)) {
    cbp_.SetRootData(game_data_->CbpRootDir());
    IK_INFO(game_data_->GameName(), "Creating {0} Layer instance ... ", game_data_->GameName().c_str());
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN(game_data_->GameName(), "Destroying {0} Layer instance !!! ", game_data_->GameName().c_str());
  }
  
  void RendererLayer::Attach() {
    IK_INFO(game_data_->GameName(), "Attaching {0} Layer instance", game_data_->GameName().c_str());
    cbp_.AddFavouritPaths(game_data_->FavDirecotries());
    
    // Decorate the Imgui Change the font of imgui
    ImguiAPI::ChangeFont(game_data_->RegularFontData(), game_data_->BoldFontData());
    ImguiAPI::SetLightGreyThemeColors();
  }
  
  void RendererLayer::Detach() {
    IK_WARN(game_data_->GameName(), "Detaching {0} Layer instance ", game_data_->GameName().c_str());
  }
  
  void RendererLayer::Update(Timestep ts) {
  }
  
  void RendererLayer::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(RendererLayer::KeyPressed));
  }
  
  void RendererLayer::RenderGui() {
    if (is_playing_) {
      
    }
    else {
      ImguiAPI::StartDcocking();
      
      ShowMenu();
      GamePlayButton();
      
      if (active_scene_) {
        ShowSettings();
        
        Renderer::Framerate(&setting_.frame_rate.flag);
        Renderer::RenderStatsGui(&setting_.stats.flag, true);
        Renderer::Render2DStatsGui(&setting_.stats_2d.flag);

        cbp_.RenderGui(&setting_.cbp.flag);
        
        if (active_scene_->IsEditing()) {
          SaveScene();
        }
      }

      ImguiAPI::EndDcocking();
    }
  }
  
  void RendererLayer::ShowSettings() {
    ImGui::Begin("Settings");
    
    SettingWrapper* setting_data = ((SettingWrapper*)&setting_);
    static uint32_t loop_limit = sizeof(Setting) /  sizeof(SettingWrapper);
    for (uint32_t setting_idx = 0; setting_idx < loop_limit; setting_idx++) {
      PropertyGrid::CheckBox((setting_data + setting_idx)->name.c_str(), (setting_data + setting_idx)->flag);
    }

    ImGui::End();
  }
  
  void RendererLayer::GamePlayButton() {
    static std::shared_ptr<Texture> play_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/play.png"));
    ImGui::Begin("Game Play", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    float size = ImGui::GetWindowHeight() - 12.0f; // 12 just random number
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    
    // Button action
    if (PropertyGrid::ImageButton("Game Play", play_texture->GetRendererID(), { size, size })) {
      SetPlay(true);
    }
    PropertyGrid::HoveredMsg("Play Button for Game");
    ImGui::End();
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
            ImguiAPI::SetLightThemeColors();
          }
          if (ImGui::MenuItem("Dark", nullptr)) {
            ImguiAPI::SetDarkThemeColors();
          }
          if (ImGui::MenuItem("Grey", nullptr)) {
            ImguiAPI::SetGreyThemeColors();
          }
          if (ImGui::MenuItem("Light Grey", nullptr)) {
            ImguiAPI::SetLightGreyThemeColors();
          }
          ImGui::EndMenu(); // ImGui::BeginMenu("Theme")
        } // if (ImGui::BeginMenu("Theme"))
        ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
      } // if (ImGui::BeginMenu("Properties"))
      
      // -------------------------------------------------------------
      //  Setting Menu
      // -------------------------------------------------------------
      if (ImGui::BeginMenu("Setting", active_scene_ and active_scene_->IsEditing())) {
        SettingWrapper* setting_data = ((SettingWrapper*)&setting_);
        static uint32_t loop_limit = sizeof(Setting) /  sizeof(SettingWrapper);
        for (uint32_t setting_idx = 0; setting_idx < loop_limit; setting_idx++) {
          Setting::UpdateSetting((setting_data + setting_idx)->name.c_str(), (setting_data + setting_idx)->flag);
        }

        ImGui::EndMenu(); // ImGui::BeginMenu("Setting")
      }
      ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    } // if (ImGui::BeginMenuBar())
  }
  
  bool RendererLayer::KeyPressed(KeyPressedEvent& event) {
    if (event.GetRepeatCount() > 0)
      return false;

    bool cmd   = Input::IsKeyPressed(KeyCode::LeftSuper) or Input::IsKeyPressed(KeyCode::RightSuper);

    if (cmd) {
      switch (event.GetKeyCode()) {
        // Scene Manager
        case KeyCode::N: NewScene();   break;
        case KeyCode::X: CloseScene(); break;
          
        default: break;
      };
    }
    return false;
  }
  
  const void RendererLayer::NewScene(const std::string& scene_path) {
    // Close the current scene
    CloseScene();
    
    // Create New Scene
    IK_INFO(game_data_->GameName(), "Creating New Scene {0}", scene_path.c_str());
    editor_scene_ = std::make_shared<EnttScene>(scene_path);
    active_scene_ = editor_scene_;
  }

  void RendererLayer::CloseScene() {
    if (!active_scene_)
      return;
    
    IK_INFO(game_data_->GameName(), "Closing Scene {0}", active_scene_->GetName().c_str());
    active_scene_.reset();
    active_scene_ = nullptr;
    
    editor_scene_.reset();
    editor_scene_ = nullptr;

    spm_.SetSceneContext(active_scene_.get());
  }
  
  const void RendererLayer::SaveScene() {
    if (!setting_ .save_scene.flag)
      return;
    
    ImGui::Begin("Save File", &setting_.save_scene.flag);
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

  
  void RendererLayer::SetPlay(bool is_play) {
    is_playing_ = is_play;
  }

}

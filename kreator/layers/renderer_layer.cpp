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
    if (setting_.play) {
      
    }
    else {
      ImguiAPI::StartDcocking();
      
      ShowMenu();

      ImguiAPI::EndDcocking();
    }
  }
  
  void RendererLayer::ShowMenu() {
    if (ImGui::BeginMenuBar()) {
      // -------------------------------------------------------------
      //  File Menu
      // -------------------------------------------------------------
      if (ImGui::BeginMenu("File")) {
        ImGui::EndMenu(); // ImGui::BeginMenu("File")
      } // if (ImGui::BeginMenu("File"))
      
      // -------------------------------------------------------------
      //  Property Menu
      // -------------------------------------------------------------
      if (ImGui::BeginMenu("Properties")) {
        ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
      } // if (ImGui::BeginMenu("Properties"))
      
      // -------------------------------------------------------------
      //  Setting Menu
      // -------------------------------------------------------------
      if (ImGui::BeginMenu("Setting", active_scene_ and active_scene_->IsEditing())) {
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
  
}

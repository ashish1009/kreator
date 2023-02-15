//
//  renderer_layer.hpp
//  kreator
//
//  Created by Ashish . on 07/02/23.
//

#pragma once

#include "game_data.hpp"

namespace ikan_game {
  
  using namespace ikan;

  enum class GameType {
    Editor, Mario
  };
  
  class RendererLayer : public Layer {
  public:
    struct Setting {
      bool play = false;
      
      bool cbp = true;
      
      bool frame_rate = true;
      bool viewport = true;
      bool stats = true;
      bool stats_2d = true;
      
      bool save_scene = true;
      bool show_colliders = true;
      bool show_grids = true;
      
      /// This function changes the flag of setting for menu
      /// - Parameters:
      ///   - tag: tag of menu
      ///   - flag: flag
      static void UpdateSetting(std::string tag, bool& flag) {
        if (ImGui::MenuItem(tag.c_str(), nullptr, flag))
          flag = (flag) ? false : true;
      }
    };
    
    /// Layer Default Constructor to store the name of layer
    RendererLayer(GameType type);
    /// Default Destructor
    ~RendererLayer();
    
    /// This funciton initialises the layer, calls when we push layer in the application (To be called only once)
    void Attach() override;
    /// This funciton destroy the layer, calls when we pop layer from the application (To be called only once)
    void Detach() override;
    
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// - Note: Core Application is taking care to call the RenderGui API for all Layers
    void RenderGui() override;
    /// This function updates the layer data. To be called each frame from application. NOTE Core
    /// Application is taking care to call the Update(ts) API for all Layers
    /// - Parameter ts: Time step between 2 Frames
    void Update(Timestep ts) override;
    /// This function handles all the events triggered for the layer. To be whenever an event is triggered
    /// in the window. NOTE Core Application is taking care to call the EventHandler(event) API for all Layers
    /// - Parameter event: Event (Base class) intance. Dispatch event from Event Dispatcher
    void EventHandler(Event& event) override;
    
  private:
    /// This function handles the key button press event
    /// - Parameter e key button pressed event
    bool KeyPressed(KeyPressedEvent& event);
    /// This function handles the Window resize event
    /// - Parameter e window resize event
    bool WindowResized(WindowResizeEvent& event);
    /// This function handles the mouse button event
    /// - Parameter e: mouse button pressed event
    bool MouseButtonPressed(MouseButtonPressedEvent& e);
    
    /// This function renders the play buttorn for  game
    void GamePlayButton();
    /// This function renders the play/pause buttorn for active scene
    void ScenePlayPauseButton();
    
    /// This function render the viewport captured in framebuffer
    void RenderViewport();
    /// This function renders the scene data
    void RenderScene(Timestep ts);
    
    /// This function renders the debug overlay
    void OverlayRender();
    
    // Scene Manager
    /// This function creates a new scene instance
    /// - Parameter scene_path: path of new scene
    const void NewScene(const std::string& scene_path = "Unsaved Scene");
    /// This function opens already saved scene from path
    /// - Parameter scene_file_path: scene file path
    const bool OpenScene(const std::string& scene_file_path);
    /// This function Renders an Imgui widget that helps in saving scene
    const void SaveScene();
    /// This function closes the current scene
    void CloseScene();
    
    /// This funciton Render the imguizmo
    void OnImguizmoUpdate();
    
    /// This function Render grids for 2D Entities
    /// - Note: Only render for Orthpgraphic primary camera
    void RenderGrid();
    /// This function render the menue bar
    void ShowMenu();
    /// This function renders the setting Widget
    void ShowSettings();
    /// This function set the game state
    /// - Parameter is_play: game state
    void SetPlay(bool is_play);
    /// This function wraps the scene play
    void PlayScene();
    
    // ------------------
    // Member variables
    // ------------------
    uint32_t viewport_width = Application::Get().GetWindow().GetWidth();
    uint32_t viewport_height = Application::Get().GetWindow().GetWidth();
    
    std::unique_ptr<GameData> game_data_;
    Setting settings_;
    Viewport viewport_;
    // TODO: Temp using Game DIR .Later will change to project DIR
    ContentBrowserPanel cbp_;
    ScenePanelManager spm_;
    std::shared_ptr<EnttScene> active_scene_, editor_scene_;
    
    // Theme
    enum class Theme {
      Light, Dark, Grey, LightGrey
    };
    Theme current_theme_ = Theme::LightGrey;
  };
  
}

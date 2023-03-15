//
//  renderer_layer.hpp
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#pragma once

#include "game_data.hpp"

namespace kreator {
  
  using namespace ikan;

  class RendererLayer : public Layer {
  public:
    struct Setting {
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
    /// - Parameter game_type: Type of game to render using this layer
    RendererLayer(GameType game_type);
    /// Default Destructor
    ~RendererLayer();
    
    /// This funciton initialises the layer, calls when we push layer in the application (To be called only once)
    void Attach() override;
    /// This funciton destroy the layer, calls when we pop layer from the application (To be called only once)
    void Detach() override;
    
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// - Note: Core Application is taking care to call the RenderGui API for all Layers
    void RenderGui() override;
    /// This function updates the layer data. To be called each frame from application.
    /// - Note Core Application is taking care to call the Update(ts) API for all Layers
    /// - Parameter ts: Time step between 2 Frames
    void Update(Timestep ts) override;
    /// This function handles all the events triggered for the layer. To be whenever an event is triggered in the window.
    /// - Note Core Application is taking care to call the EventHandler(event) API for all Layers
    /// - Parameter event: Event (Base class) intance. Dispatch event from Event Dispatcher
    void EventHandler(Event& event) override;
    
  private:
    // ------------------
    // Member functions
    // ------------------
    bool KeyPressed(KeyPressedEvent& event);

      // Scene Manager
    /// This function creates a new scene instance
    /// - Parameter scene_path: path of new scene
    const void NewScene(const std::string& scene_path = "Unsaved Scene");
    /// This function closes the current scene
    void CloseScene();
    /// This function opens already saved scene from path
    /// - Parameter scene_file_path: scene file path
    const bool OpenScene(const std::string& scene_file_path);
    /// This function Renders an Imgui widget that helps in saving scene
    const void SaveScene();

    /// This function render the menue bar
    void ShowMenu();

    /// This function renders the play buttorn for  game
    void GamePlayButton();

    /// This function set the game state
    /// - Parameter is_play: game state
    void SetPlay(bool is_play);
    /// This function wraps the scene play
    void PlayScene();

    // ------------------
    // Member variables
    // ------------------
    bool is_playing_ = false;

    std::unique_ptr<GameData> game_data_;
    Setting setting_;
    
    std::shared_ptr<EnttScene> active_scene_, editor_scene_;
    ScenePanelManager spm_;
    ContentBrowserPanel cbp_;
  };
  
}

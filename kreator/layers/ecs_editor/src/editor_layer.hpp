//
//  editor_layer.hpp
//  ecs_editor
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

namespace editor {
  
  using namespace ikan;
  using namespace ecs;
  
  class EditorLayer : public Layer {
  public:
    struct Setting {
      bool cbp = true;
      bool spm = true;
      
      bool frame_rate = true;
      bool viewport = true;
      bool stats = true;
      
      bool save_scene = true;
      
      /// This function changes the flag of setting for menu
      /// - Parameters:
      ///   - tag: tag of menu
      ///   - flag: flag
      static void UpdateSetting(std::string tag, bool& flag);
    };
    
    /// Layer Default Constructor to store the name of layer
    EditorLayer();
    /// Default Destructor
    ~ EditorLayer();
    
    /// This funciton initialises the layer, calls when we push layer in the application (To be called only once)
    void Attach() override;
    /// This funciton destroy the layer, calls when we pop layer from the application (To be called only once)
    void Detach() override;
    
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// NOTE Core Application is taking care to call the RenderGui API for all Layers
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
    // --------------------
    // Member functions
    // --------------------
    /// This function render the menue bar
    void ShowMenu();
    /// This function handles the mouse button event
    /// - Parameter e: mouse button pressed event
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    /// This function handles the key button press event
    /// - Parameter e key button pressed event
    bool OnKeyPressed(KeyPressedEvent& event);
    /// This funciton Render the imguizmo
    void OnImguizmoUpdate();
    
    // Scene Manager
    /// This function creates a new scene instance
    /// - Parameter scene_path: path of new scene
    const void NewScene(const std::string& scene_path = "Unsaved Scene");
    /// This function opens already saved scene from path
    /// - Parameter scene_file_path: scene file path
    const void OpenScene(const std::string& scene_file_path);
    /// This function Renders an Imgui widget that helps in saving scene
    const void SaveScene();
    /// This function closes the current scene
    void CloseScene();

    // --------------------
    // Member variables
    // --------------------
    Viewport viewport_;
    ContentBrowserPanel cbp_;
    ScenePanelManager spm_;
    std::shared_ptr<EnttScene> active_scene_;
    
    Setting setting_;

    // Font
    bool change_font_ = true;
    std::string current_font_name_ = "OpensansRegular";
    std::string current_font_path_ = "fonts/Opensans/Regular.ttf";
    std::string current_bold_font_name_ = "OpensansBold";
    std::string current_bold_font_path_ = "fonts/Opensans/Bold.ttf";
    
    // Theme
    enum class Theme {
      Light, Dark, Grey, LightGrey
    };
    Theme current_theme_ = Theme::LightGrey;
  };
  
}

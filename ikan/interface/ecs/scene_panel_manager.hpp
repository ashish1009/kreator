//
//  scene_panel_manager.hpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#pragma once


namespace ikan {
  
  class EnttScene;
  
  /// This class is the wrapper to render scene pannel
  class ScenePanelManager {
  public:
    struct Setting {
      bool scene_panel = true;
      bool property_panel = true;
    };

    /// This Constructor creates Scene pannel manager instance
    ScenePanelManager(EnttScene* context = nullptr);
    /// This Destructor destroys Scene pannel manager instance
    ~ScenePanelManager();
  
    /// This function update the scene contexrt
    /// - Parameter context: scene context
    void SetSceneContext(EnttScene* context);
    /// This function renders imgui window for scene pannels
    void RenderGui();

  private:
    // Member functions
    /// This function render a pannel for scene information
    void ScenePannel();
    /// This function renders the property for selected entityu in scene
    void PropertyPannel();

    // Member Variables
    EnttScene* scene_context_;
    Setting setting_;
  };
  
}

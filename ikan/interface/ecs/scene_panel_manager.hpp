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
    /// This Constructor creates Scene pannel manager instance
    ScenePanelManager(EnttScene* context = nullptr);
    /// This Destructor destroys Scene pannel manager instance
    ~ScenePanelManager();
  
    /// This function update the scene contexrt
    /// - Parameter context: scene context
    void SetSceneContext(EnttScene* context);

  private:
    // Member Variables
    EnttScene* scene_context_;
  };
  
}

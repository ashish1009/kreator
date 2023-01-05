//
//  scene_panel_manager.hpp
//  ikan
//
//  Created by Ashish . on 05/01/23.
//

#pragma once

// This class includes the Scene Pannel manager class

#include "scene/entity.hpp"

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
    
    /// This function renders imgui window for scene pannels
    /// - Parameter is_opened: flag to show or hide the widget
    void RenderGui(bool* is_opened = nullptr);
    
    DELETE_COPY_MOVE_CONSTRUCTORS(ScenePanelManager)

  private:
    /// This function render a pannel for scene information
    void ScenePannel();
    /// This function draw the entity tree node
    /// - Parameter entity_id: entity id from scene registry
    void DrawEntityTreeNode(entt::entity entity_id);

    // Member Variables
    EnttScene* scene_context_;
    Entity selected_entity_;
  };
  
}

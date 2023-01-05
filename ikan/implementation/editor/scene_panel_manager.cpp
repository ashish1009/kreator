//
//  scene_panel_manager.cpp
//  ikan
//
//  Created by Ashish . on 05/01/23.
//

#include "scene_panel_manager.hpp"

namespace ikan {
  
  ScenePanelManager::ScenePanelManager(EnttScene* context) : scene_context_(context) {
    IK_CORE_TRACE(LogModule::ScenePanelManager, "Creating Scene pannel Manager ...");
  }
  
  ScenePanelManager::~ScenePanelManager() {
    IK_CORE_TRACE(LogModule::ScenePanelManager, "Destroying Scene pannel Manager !!!");
  }
  
  void ScenePanelManager::SetSceneContext(EnttScene *context) {
    scene_context_ = context;
  }
  
  void ScenePanelManager::RenderGui(bool* is_opened) {
    if (!is_opened) return;
    if (*is_opened == false) return;

    ImGui::Begin("Scene Manager", is_opened);
    ImGui::PushID("Scene Manager");

    ImGui::PopID(); // Scene Manager
    ImGui::End();   // Scene Manager
  }
  
}

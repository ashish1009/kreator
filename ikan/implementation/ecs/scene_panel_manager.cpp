//
//  scene_panel_manager.cpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#include "scene_panel_manager.hpp"

namespace ikan {
  
  ScenePanelManager::ScenePanelManager(EnttScene* context) : scene_context_(context) {
    IK_CORE_TRACE(LogModule::ScenePanelManager, "Creating Scene pannel Manager ...");
  }
  
  ScenePanelManager::~ScenePanelManager() {
    IK_CORE_TRACE(LogModule::ScenePanelManager, "Destroying Scene pannel Manager !!!");
  }
  
  void ScenePanelManager::RenderGui() {
    ScenePannel();
    PropertyPannel();
  }
  
  void ScenePanelManager::ScenePannel() {
    if (!setting_.scene_panel)
      return;

    ImGui::Begin("Scene Manager", &setting_.scene_panel);
    ImGui::PushID("Scene Manager");

    ImGui::PopID();
    ImGui::End();

  }
  
  void ScenePanelManager::PropertyPannel() {
    if (!setting_.property_panel)
      return;

    ImGui::Begin("Entity Property", &setting_.property_panel);
    ImGui::PushID("Entity Property");

    ImGui::PopID();
    ImGui::End();
  }

  void ScenePanelManager::SetSceneContext(EnttScene* context) { scene_context_ = context; }
}

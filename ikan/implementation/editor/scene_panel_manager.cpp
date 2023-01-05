//
//  scene_panel_manager.cpp
//  ikan
//
//  Created by Ashish . on 05/01/23.
//

#include "scene_panel_manager.hpp"
#include "scene/entity.hpp"
#include "scene/core_components.hpp"
#include "editor/property_grid.hpp"

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

    ScenePannel();

    ImGui::PopID(); // Scene Manager
    ImGui::End();   // Scene Manager
    
    ImGui::Begin("Entity Property");
    ImGui::PushID("Entity Property");
    
    if (selected_entity_)
      PropertyPannel();
    
    ImGui::PopID(); // Entity Property
    ImGui::End();   // Entity Property

  }
  
  void ScenePanelManager::ScenePannel() {
    scene_context_->registry_.each([&](auto entity_id)
                                       {
      DrawEntityTreeNode(entity_id);
    });
  }
  
  void ScenePanelManager::PropertyPannel() {
    // ----------
    // Tag
    // ----------
    auto& tag = selected_entity_.GetComponent<TagComponent>().tag;
    PropertyGrid::TextBox(tag);
    PropertyGrid::HoveredMsg(("Entity ID : " + std::to_string((uint32_t)selected_entity_)).c_str());
    
//    auto text_box_size = ImGui::GetItemRectSize();
  }
  
  void ScenePanelManager::DrawEntityTreeNode(entt::entity entity_id) {
    Entity& entity = scene_context_->entity_id_map_.at(entity_id);
    ImGuiTreeNodeFlags flags = ((selected_entity_ == entity) ?
                                ImGuiTreeNodeFlags_Selected : 0)
    | ImGuiTreeNodeFlags_SpanAvailWidth;
    const std::string& tag = entity.GetComponent<TagComponent>().tag;
    bool opened = ImGui::TreeNodeEx((void*)(tag.c_str()),
                                    flags,
                                    tag.c_str());

    // TODO: Add Hovered message Feature later
    
    // Update the selected entity if item is clicked
    if (ImGui::IsItemClicked() or ImGui::IsItemClicked(1))
      selected_entity_ = entity;

    if (opened) {
      // TODO: Add Feature
      ImGui::TreePop();
    }

  }
  
}

//
//  scene_panel_manager.hpp
//  ikan
//
//  Created by Ashish . on 05/01/23.
//

#pragma once

// This class includes the Scene Pannel manager class

#include "ecs/core_components.hpp"
#include "ecs/entity.hpp"

namespace ecs {
  
  using namespace ikan;
  
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
    
    /// This Function update the selected entity of scene
    /// - Parameter entity: new selected entity
    void SetSelectedEntity(Entity* entity);
    /// This Function update the selected entity of scene
    Entity* GetSelectedEntity();
    /// This function returns the context of panel
    EnttScene* GetContext();
    /// This function returns the reference of setting
    Setting& GetSetting();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(ScenePanelManager)

  private:
    /// This function render a pannel for scene information
    void ScenePannel();
    /// This function renders the property for selected entityu in scene
    void PropertyPannel();
    /// This function draw the entity tree node
    /// - Parameter entity_id: entity id from scene registry
    void DrawEntityTreeNode(entt::entity entity_id);
    /// This function renders the option menu on right click the empty space
    void RightClickOptions();
    /// This function adds the component in the selected entity
    void AddComponent();
    
    template <typename T>
    /// This function create menu for add component
    /// - Parameters:
    ///   - menu_item: menu name
    ///   - exclusive_func: exclusing function
    void AddComponentMenu(const std::string& menu_item, std::function<bool()> exclusive_func = nullptr) {
      if (!exclusive_func) {
        exclusive_func = [this]() {
          return selected_entity_.HasComponent<T>();
        };
      }
      if (ImGui::MenuItem(menu_item.c_str(),
                          nullptr, // Shortcut
                          false, // Selected
                          !exclusive_func())) {
        selected_entity_.AddComponent<T>();
        ImGui::CloseCurrentPopup();
      }
    }

    // Member Variables
    EnttScene* scene_context_;
    Entity selected_entity_;
    Setting setting_;

    bool delete_entity_ = false;
  };
  
}

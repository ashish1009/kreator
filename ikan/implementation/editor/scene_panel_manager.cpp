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
  
  /// This function renders the components in property pannel. Takes the function pointer in argument
  /// - Parameters:
  ///   - name: name of the entity
  ///   - entity: entity reference pointer
  ///   - ui_function: ui function to be called
  template<typename T, typename UIFunction> static void DrawComponent(const std::string& name,
                                                                      const Entity& entity,
                                                                      UIFunction ui_function) {
    // Flag for rendering the title of entity
    const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen |
    ImGuiTreeNodeFlags_Framed |
    ImGuiTreeNodeFlags_SpanAvailWidth |
    ImGuiTreeNodeFlags_AllowItemOverlap |
    ImGuiTreeNodeFlags_FramePadding;
    
    // Render the property if entity have a component component
    if (entity.HasComponent<T>()) {
      // Title style
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
      
      // Render the title named as entity name
      bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(),
                                    tree_node_flags,
                                    name.c_str());
      ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
      
      // Get the avilable width and height for button position
      ImVec2 content_region_available = ImGui::GetContentRegionAvail();
      float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
      
      // Set the curson position on same line for (X) button
      ImGui::SameLine(content_region_available.x - line_height * 0.5f);
      float content_height = GImGui->Font->FontSize;
      
      const auto& current_cursor_pos = ImGui::GetCursorPos();
      ImGui::SetCursorPos({current_cursor_pos.x, current_cursor_pos.y + content_height / 4});
      
      // Render the button (X) for removing the component
      static std::shared_ptr<Texture> close_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/gear.png"));
      if (PropertyGrid::ImageButton("Close",
                                    close_texture->GetRendererID(),
                                    { content_height, content_height } // Size
                                    )) {
        ImGui::OpenPopup("ComponentSettings");
      }
      
      // Pop up for removing the component
      bool remove_component = false;
      if (ImGui::BeginPopup("ComponentSettings")) {
        if (ImGui::MenuItem("Remove component"))
          remove_component = true;
        
        ImGui::EndPopup();
      }
      
      // Call the function pointer
      if (open) {
        auto& component = entity.GetComponent<T>();
        ui_function(component);
        ImGui::TreePop();
      }
      
      // Remove the component if button clicked
      if (remove_component)
        entity.RemoveComponent<T>();
    } // if (entity.HasComponent<T>())
  }
  
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
    
    // Reset the selected entity if mouse is clicked on empty space
    if (ImGui::IsMouseDown((int32_t)MouseButton::ButtonLeft) and ImGui::IsWindowHovered()) {
      selected_entity_ = {};
    }
  }
  
  void ScenePanelManager::PropertyPannel() {
    // Tag
    auto& tag = selected_entity_.GetComponent<TagComponent>().tag;
    PropertyGrid::TextBox(tag);
    PropertyGrid::HoveredMsg(("Entity ID : " + std::to_string((uint32_t)selected_entity_)).c_str());
    ImGui::Separator();
    
    // Draw other components
    DrawComponent<TransformComponent>("Transform", selected_entity_, [](auto& tc) { tc.RenderGui(); });
    DrawComponent<QuadComponent>("Qaad", selected_entity_, [](auto& qc) { qc.RenderGui(); });
    DrawComponent<CircleComponent>("Circle", selected_entity_, [this](auto& cc) { cc.RenderGui(); });
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
  
  void ScenePanelManager::SetSelectedEntity(Entity* entity) {
    if (entity)
      selected_entity_ = *entity;
    else if (selected_entity_)
      selected_entity_ = {};
  }
  
}

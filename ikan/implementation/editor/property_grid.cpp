//
//  property_grid.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "property_grid.hpp"
#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  void PropertyGrid::HoveredMsg(const char* description, bool error) {
    if (ImGui::IsItemHovered()) {
      if (error)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(description);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
      
      if (error)
        ImGui::PopStyleColor();
    }
  }
  
  void PropertyGrid::HelpMarker(const char* description,
                                const char* icon_char,
                                bool same_line) {
    if (same_line)
      ImGui::SameLine();
    
    ImGui::TextDisabled(icon_char);
    HoveredMsg(description);
  }

  bool PropertyGrid::Search(char* value,
                            const char* hint,
                            const glm::vec2& size) {
    bool modified = false;
    ImGui::PushID("Search");
    ImGui::PushItemWidth(-1);
    
    static std::shared_ptr<Texture> search_exture = Renderer::GetTexture(AM::CoreAsset("textures/icons/search.png"));
    PropertyGrid::Image(INT2VOIDP(search_exture->GetRendererID()),
                        size,
                        glm::vec2( 0, 1 ),
                        glm::vec2( 1, 0 ));
    
    ImGui::SameLine();
    
    // Copy the Name of entity to buffer that will be dumy text in property pannel
    char buffer[256];
    strcpy(buffer, value);
    
    if (ImGui::InputTextWithHint("", hint, buffer, IM_ARRAYSIZE(buffer))) {
      strcpy(value, buffer);
      modified = true;
    }
    
    ImGui::PopItemWidth();
    ImGui::PopID();
    
    return modified;
  }
  
  void PropertyGrid::Image(void* textureID,
                           const glm::vec2& size,
                           const glm::vec2& uv0,
                           const glm::vec2& uv1) {
    auto& colors = ImGui::GetStyle().Colors;
    const auto& button_hovered = colors[ImGuiCol_ButtonHovered];
    
    // ------------------------------------------
    // No Background color
    // ------------------------------------------
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(button_hovered.x,
                                 button_hovered.y,
                                 button_hovered.z,
                                 0.5f));
    
    ImGui::Image((void*)textureID,
                 ImVec2{ size.x, size.y },
                 ImVec2{ uv0.x, uv0.y },
                 ImVec2{ uv1.x, uv1.y });
    
    ImGui::PopStyleColor(1);
  }
  
  bool PropertyGrid::ImageButton(int32_t lable_id,
                                 uint32_t texId,
                                 const glm::vec2& size) {
    bool result = false;
    ImTextureID my_texture_id = (ImTextureID)((size_t)texId);
    ImGui::PushID(lable_id);
    result = ImGui::ImageButton(my_texture_id,
                                { size.x, size.y },
                                ImVec2(0.0f, 1.0f),
                                ImVec2(1.0f, 0.0f));
    ImGui::PopID();
    
    return result;
  }
  
  bool PropertyGrid::ImageButton(const std::string& lable_id,
                                 uint32_t texId,
                                 const glm::vec2& size,
                                 const glm::vec4& bgColor,
                                 int32_t padding) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    
    auto& colors = ImGui::GetStyle().Colors;
    const auto& button_hovered = colors[ImGuiCol_ButtonHovered];
    
    // ------------------------------------------
    // No Background color
    // ------------------------------------------
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(button_hovered.x, button_hovered.y, button_hovered.z, 0.5f));
    const auto& button_cctive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(button_cctive.x, button_cctive.y, button_cctive.z, 0.5f));
    
    bool result = false;
    ImTextureID my_texture_id = (ImTextureID)((size_t)texId);
    ImGui::PushID(lable_id.c_str());
    result = ImGui::ImageButton(my_texture_id,
                                { size.x, size.y },
                                ImVec2(0.0f, 1.0f),
                                ImVec2(1.0f, 0.0f),
                                padding,
                                { bgColor.r, bgColor.g, bgColor.b, bgColor.a });
    ImGui::PopID();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    
    return result;
  }
  
}

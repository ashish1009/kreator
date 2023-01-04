//
//  property_grid.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "property_grid.hpp"
#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  void PropertyGrid::HoveredMsg(const char* description) {
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(description);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
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
    const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
    
    // ------------------------------------------
    // No Background color
    // ------------------------------------------
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(buttonHovered.x,
                                 buttonHovered.y,
                                 buttonHovered.z,
                                 0.5f));
    
    ImGui::Image((void*)textureID,
                 ImVec2{ size.x, size.y },
                 ImVec2{ uv0.x, uv0.y },
                 ImVec2{ uv1.x, uv1.y });
    
    ImGui::PopStyleColor(1);
  }
}

//
//  property_grid.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "property_grid.hpp"

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

  
}

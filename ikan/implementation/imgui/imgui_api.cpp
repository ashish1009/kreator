//
//  imgui_api.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "imgui_api.hpp"

namespace ikan {
  
  void ImguiAPI::Framerate() {
    ImGui::Begin("Frame Rate");
    ImGui::PushID("Frame Rate");
    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    ImGui::PopID();
    ImGui::End();
  }
  
}

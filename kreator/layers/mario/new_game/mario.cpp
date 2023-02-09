//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 08/02/23.
//

#include "mario.hpp"

namespace mario {
  
  void MarioData::Init() {
    BatchRenderer::Init(2000, 0, 0);
  }
  
  void MarioData::Update(Timestep ts) {
    
  }
  
  void MarioData::RenderGui() {
    ImGui::Begin("Mario Data");
    ImGui::End();
    
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Mario")) {
        ImGui::EndMenu(); // ImGui::BeginMenu("File")
      } // if (ImGui::BeginMenu("File"))
      ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    } // if (ImGui::BeginMenuBar())
  }
  
  std::string MarioData::GameName() {
    return "Mario";
  }
  
  ImguiFont MarioData::RegularFontData() {
    return {
      AM::ClientAsset("fonts/mario.ttf"),
      14.0f
      /* Size of font */
    };
  }
  
  ImguiFont MarioData::BoldFontData() {
    return {
      AM::ClientAsset("fonts/mario.ttf"),
      14.0f /* Size of font */
    };
  }
  
  std::vector<std::filesystem::path> MarioData::FavDirecotries() {
    return {
      AM::ProjectPath("kreator/layers/mario/assets"),
      AM::ProjectPath("kreator/layers/mario/assets/scenes"),
    };
  }

  
}

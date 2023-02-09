//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 08/02/23.
//

#include "mario.hpp"

namespace mario {
  
  MarioData::MarioData(const Viewport* const viewport)
  : viewport_(viewport) {
  }
  
  void MarioData::Init() {
    BatchRenderer::Init(2000, 0, 0);
  }
  
  void MarioData::Update(Timestep ts) {
  }
  
  void MarioData::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(MarioData::MouseButtonPressed));
  }
    
  bool MarioData::MouseButtonPressed(MouseButtonPressedEvent &e) {
    IK_INFO("", "{0}, {1}", viewport_->mouse_pos_x, viewport_->mouse_pos_y);
    return false;
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

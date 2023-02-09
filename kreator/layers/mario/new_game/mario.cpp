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
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(MarioData::KeyPressEvent));
  }
    
  bool MarioData::KeyPressEvent(KeyPressedEvent &e) {
    if (!is_playing_) {
      bool shift = Input::IsKeyPressed(KeyCode::LeftShift) or Input::IsKeyPressed(KeyCode::RightShift);
      if (shift) {
        if (Entity* entity = panel_->GetSelectedEntity(); entity) {
          auto& tc = entity->GetComponent<TransformComponent>();
          switch (e.GetKeyCode()) {
            case KeyCode::D: {
              scene_->DuplicateEntity(*entity);
              break;
            }
            case KeyCode::Backspace: {
              scene_->DestroyEntity(*entity);
              panel_->SetSelectedEntity(nullptr);
              break;
            }
            case KeyCode::Left: {
              tc.UpdateTranslation_X(tc.Translation().x - 1.0f);
              break;
            }
            case KeyCode::Right: {
              tc.UpdateTranslation_X(tc.Translation().x + 1.0f);
              break;
            }
            case KeyCode::Up: {
              tc.UpdateTranslation_Y(tc.Translation().y + 1.0f);
              break;
            }
            case KeyCode::Down: {
              tc.UpdateTranslation_Y(tc.Translation().y - 1.0f);
              break;
            }
            default:
              break;
          } // switch (e.GetKeyCode())
        } // if (entity)
      } // if (shift)
    }
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
  
  void MarioData::SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) {
    scene_ = scene;
    panel_ = panel;
  }
  
  void MarioData::SetState(bool is_playing)  {
    is_playing_ = is_playing;
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

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
    if (!is_playing_)
      RenderSelectedRectangle();
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

  void MarioData::RenderSelectedRectangle() {
    if (!(viewport_->mouse_pos_x >= 0 and viewport_->mouse_pos_y >= 0 and
          viewport_->mouse_pos_x <= viewport_->width and viewport_->mouse_pos_y <= viewport_->height))
      return;
    
    static bool first_clicked = true;
    static glm::vec2 initial_block_position_ = glm::vec2(0.0f);
    static glm::vec2 final_block_position_ = glm::vec2(0.0f);
    
    static int32_t initial_x = 0, initial_y = 0;
    static int32_t final_x = 0, final_y = 0;
    
    if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft)) {
      const auto& cd = scene_->GetPrimaryCameraData();
      float zoom = cd.scene_camera->GetZoom();
      float aspect_ratio = cd.scene_camera->GetAspectRatio();
      
      if (first_clicked) {
        first_clicked = false;
        initial_block_position_ = {
          viewport_->mouse_pos_x - ((float)viewport_->width / 2),
          viewport_->mouse_pos_y - ((float)viewport_->height / 2)
        };
        initial_block_position_ *= ((zoom * aspect_ratio) / viewport_->width);
        
        initial_x = std::floor(initial_block_position_.x + 0.5f);
        initial_y = std::floor(initial_block_position_.y + 0.5f);
      }
      
      final_block_position_ = {
        viewport_->mouse_pos_x - ((float)viewport_->width / 2),
        viewport_->mouse_pos_y - ((float)viewport_->height / 2)
      };
      final_block_position_ *= ((zoom * aspect_ratio) / viewport_->width);
      
      final_x = std::floor(final_block_position_.x + 0.5f);
      final_y = std::floor(final_block_position_.y + 0.5f);
      
      BatchRenderer::BeginBatch(scene_->GetPrimaryCameraData().scene_camera->GetProjection() *
                                glm::inverse(scene_->GetPrimaryCameraData().transform_matrix));
      BatchRenderer::DrawRect({initial_block_position_.x + cd.position.x, initial_block_position_.y + cd.position.y, 0.1},
                              {final_block_position_.x + cd.position.x, final_block_position_.y + cd.position.y, 0.1},
                              {1, 1, 1, 1});
      BatchRenderer::EndBatch();
      
    }
    if (Input::IsMouseButtonReleased(MouseButton::ButtonLeft)) {
      if (!first_clicked) {
        // Store the selected Entity
        {
          int32_t min_x = std::min(initial_x, final_x);
          int32_t max_x = std::max(initial_x, final_x);
          int32_t min_y = std::min(initial_y, final_y);
          int32_t max_y = std::max(initial_y, final_y);
          for (int x = min_x; x <= max_x; x++) {
            for (int y = min_y; y <= max_y; y++) {
            }
          }
        }
      }
      first_clicked = true;
    }
  }
  
}

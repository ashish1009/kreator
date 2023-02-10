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
      StoreSelectedEntities();
  }
  
  void MarioData::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(MarioData::KeyPressEvent));
  }
    
  bool MarioData::KeyPressEvent(KeyPressedEvent &e) {
    if (!is_playing_) {
      bool shift = Input::IsKeyPressed(KeyCode::LeftShift) or Input::IsKeyPressed(KeyCode::RightShift);
      if (shift) {
        switch (e.GetKeyCode()) {
          case KeyCode::D:          DuplicateSelectedEntities();  break;
          case KeyCode::Backspace:  DeleteSelectedEntities();     break;
          case KeyCode::Escape:     ClearSelectedEntities();      break;
            
          case KeyCode::Left:     MoveEntities(Left);   break;
          case KeyCode::Right:    MoveEntities(Right);  break;
          case KeyCode::Up:       MoveEntities(Up);     break;
          case KeyCode::Down:     MoveEntities(Down);   break;
            
          default: break;
        } // switch (e.GetKeyCode())
      } // if (shift)
    }
    return false;
  }
  
  void MarioData::RenderGui() {
    ImGui::Begin("Mario Data");
    
    for (Entity* entity : selected_entities_) {
      ImGui::Text("%s", entity->GetComponent<TagComponent>().tag.c_str());
    }
    
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

  void MarioData::StoreSelectedEntities() {
    if (!(viewport_->mouse_pos_x >= 0 and viewport_->mouse_pos_y >= 0 and
          viewport_->mouse_pos_x <= viewport_->width and viewport_->mouse_pos_y <= viewport_->height))
      return;
    
    static bool first_clicked = true;
    
    static glm::vec2 initial_mouse_position_ = glm::vec2(0.0f);
    static glm::vec2 final_mouse_position_ = glm::vec2(0.0f);

    static glm::vec2 initial_block_position_ = glm::vec2(0.0f);
    static glm::vec2 final_block_position_ = glm::vec2(0.0f);
    
    const auto& cd = scene_->GetPrimaryCameraData();
    float zoom = cd.scene_camera->GetZoom();
    float aspect_ratio = cd.scene_camera->GetAspectRatio();

    if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft)) {
      if (first_clicked) {
        ClearSelectedEntities();
        
        first_clicked = false;
        initial_mouse_position_ = { viewport_->mouse_pos_x, viewport_->mouse_pos_y };
        initial_block_position_ = {
          viewport_->mouse_pos_x - ((float)viewport_->width / 2),
          viewport_->mouse_pos_y - ((float)viewport_->height / 2)
        };
        initial_block_position_ *= ((zoom * aspect_ratio) / viewport_->width);
      }
      
      final_mouse_position_ = { viewport_->mouse_pos_x, viewport_->mouse_pos_y };
      final_block_position_ = {
        viewport_->mouse_pos_x - ((float)viewport_->width / 2),
        viewport_->mouse_pos_y - ((float)viewport_->height / 2)
      };
      final_block_position_ *= ((zoom * aspect_ratio) / viewport_->width);
      
      // Render the outline rectangle
      BatchRenderer::BeginBatch(scene_->GetPrimaryCameraData().scene_camera->GetProjection() *
                                glm::inverse(scene_->GetPrimaryCameraData().transform_matrix));
      BatchRenderer::DrawRect({initial_block_position_.x + cd.position.x, initial_block_position_.y + cd.position.y, 0.1},
                              {final_block_position_.x + cd.position.x, final_block_position_.y + cd.position.y, 0.1},
                              {1, 1, 1, 1});
      BatchRenderer::EndBatch();
      
    }
    if (Input::IsMouseButtonReleased(MouseButton::ButtonLeft)) {
      if (!first_clicked) {
        // Store entites present in selected entitity
        float block_size_x = viewport_->width / (zoom * aspect_ratio);
        float block_size_y = viewport_->height / zoom;
                
        float min_x = std::min(initial_mouse_position_.x, final_mouse_position_.x);
        float max_x = std::max(initial_mouse_position_.x, final_mouse_position_.x);
        float min_y = std::min(initial_mouse_position_.y, final_mouse_position_.y);
        float max_y = std::max(initial_mouse_position_.y, final_mouse_position_.y);

        for (float i_x = min_x; i_x < (max_x + block_size_x); i_x += block_size_x) {
          for (float i_y = min_y; i_y < (max_y + block_size_y); i_y += block_size_y) {
            // Get pixel from rednerer
            int32_t pixel = -1;
            Renderer::GetEntityIdFromPixels(i_x, i_y, viewport_->framebuffer->GetPixelIdIndex(), pixel);
            
            if (scene_) {
              if (pixel <= (int32_t)scene_->GetMaxEntityId())
                selected_entities_.push_back(scene_->GetEnitityFromId(pixel));
            }
          }
        }
        
        // TODO: Do it while adding to vector? or keep separate???
        HighlightSelectedEntities(true);
      }
      first_clicked = true;
    }
  }

  void MarioData::MoveEntities(Direction direction) {
    for (Entity* entity : selected_entities_) {
      auto& tc = entity->GetComponent<TransformComponent>();
      switch (direction) {
        case Down:      tc.UpdateTranslation_Y(tc.Translation().y - 1.0f);     break;
        case Up:        tc.UpdateTranslation_Y(tc.Translation().y + 1.0f);     break;
        case Right:     tc.UpdateTranslation_X(tc.Translation().x + 1.0f);     break;
        case Left:      tc.UpdateTranslation_X(tc.Translation().x - 1.0f);     break;
        default: break;
      }
    }
  }

  void MarioData::HighlightSelectedEntities(bool enable) {
    for (Entity* entity : selected_entities_) {
      auto& tc = entity->GetComponent<TransformComponent>();
      auto& qc = entity->GetComponent<QuadComponent>();
      
      if (enable) {
        tc.UpdateTranslation_Z(0.1f);
        qc.color.a -=0.2f;
      }
      else {
        tc.UpdateTranslation_Z(0.0f);
        qc.color.a +=0.2f;
      }
    }
  }
  
  void MarioData::ClearSelectedEntities() {
    HighlightSelectedEntities(false);
    selected_entities_.clear();
  }
  
  void MarioData::DeleteSelectedEntities() {
    for (Entity* entity : selected_entities_) {
      if (panel_->GetSelectedEntity() and *(panel_->GetSelectedEntity()) == *entity) {
        panel_->SetSelectedEntity(nullptr);
      }
      scene_->DestroyEntity(*entity);
    }    
    selected_entities_.clear();
  }
  
  void MarioData::DuplicateSelectedEntities() {
    for (Entity* entity : selected_entities_) {
      scene_->DuplicateEntity(*entity);
    }
  }

}

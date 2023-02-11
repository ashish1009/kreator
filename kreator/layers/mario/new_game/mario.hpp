//
//  mario.hpp
//  kreator
//
//  Created by Ashish . on 08/02/23.
//

#pragma once

#include "common.h"

#include "renderer_layer.hpp"
#include "player.hpp"

namespace mario {
  
  using namespace ikan;
  
  static const uint32_t MaxTime = 100;
  
  struct TextData {
    glm::vec2 size = { 0.6f, 0.6f };
    const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glm::mat4 still_camera_projection;
    float pos_row[2];
    float pos_col[4];
    
    void Update(float width, float height) {
      still_camera_projection = glm::ortho( 0.0f, (float)width, 0.0f, (float)height);
    
      static const glm::vec2 size_ref = { 0.6f, 0.6f };
      size = size_ref * width / 1600.0f;
      
      pos_row[0] = (float)height - 50;
      pos_row[1] = (float)height - 80;
      
      pos_col[0] = 50.0f;
      pos_col[1] = ((float)width * (1.0f / 4.0f)) + 50.0f;
      pos_col[2] = ((float)width * (2.0f / 4.0f)) + 50.0f;
      pos_col[3] = ((float)width * (3.0f / 4.0f)) + 50.0f;
    }
    
    void Render(const std::string& title, uint32_t row, uint32_t col) {
      TextRenderer::RenderText(title, { pos_col[col], pos_row[row], 0.3f }, size, color);
    }
  };
    
  class MarioData : public ikan_game::GameData {
  public:
    MarioData(const Viewport* const viewport);
    
    void Init(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) override;
    void Update(Timestep ts) override;
    void EventHandler(Event& event) override;
    void RenderGui() override;
    void SetState(bool is_playing) override;
    void SetViewportSize(uint32_t width, uint32_t height) override;
    
    // Getters
    glm::vec4 GetBgColor() override { return level_bg_; }
    std::string GameName() override { return "Mario"; }
    std::string SavedScene() override { return AM::ClientAsset("scenes/Mario_Scene.ikanScene"); }
    ImguiFont RegularFontData() override;
    ImguiFont BoldFontData() override;
    std::vector<std::filesystem::path> FavDirecotries() override;
    
  private:
    enum Direction {Left, Right, Up, Down};

    /// This function handles the Key button event
    /// - Parameter e: Key pressed event
    bool KeyPressEvent(KeyPressedEvent& e);
    /// This function handles the MouseButton event
    /// - Parameter e: MouseButton pressed event
    bool MouseButtonPressEvent(MouseButtonPressedEvent& e);

    /// This function render the rectangle when clicg drag mouse
    void StoreSelectedEntities();
    /// This function moves the selected entities
    /// - Parameter direction: Direction of movement
    void MoveEntities(Direction direction);
    /// This function highlight the selected entiies
    /// - Parameter enable: flag to set or unset
    void HighlightSelectedEntities(bool enable);
    /// This function clear the selected entities
    void ClearSelectedEntities();
    /// This function Delete the selected entities
    void DeleteSelectedEntities();
    /// This function Duplicate the selected entities
    void DuplicateSelectedEntities();

    void AddComponentHack();
    void MoveCameraDebug(Timestep ts);
    
    bool is_playing_ = false;
    
    uint32_t viewport_width_ = Application::Get().GetWindow().GetWidth();
    uint32_t viewport_height_ = Application::Get().GetWindow().GetHeight();
    
    const Viewport* const viewport_;
    std::shared_ptr<EnttScene> scene_;
    ScenePanelManager* panel_;
    
    std::unordered_map<entt::entity, Entity*> selected_entities_;
    
    Player player;
    glm::vec4 level_bg_ = {0.2, 0.4, 0.6, 1.0f};
    
    TextData text_data_;
    std::string font_path = "fonts/mario.ttf";
    
    uint32_t score_ = 0;
    uint32_t world_ = 1;
    uint32_t level_ = 1;
    uint32_t coins_ = 0;
    uint32_t time_left_ = 0;
    
    float timer_ = 0;
  };
  
}

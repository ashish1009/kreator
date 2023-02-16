//
//  mario.hpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//


#pragma once

#include "layers/ikan_game_editor/src/renderer_layer.hpp"

namespace mario {
  
  using namespace ikan;
  
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
  
  class Mario : public ikan_game::GameData {
  public:
    Mario(const Viewport* const viewport);
    ~Mario();

    void Init() override;
    void SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) override;
    void Update(Timestep ts) override;
    void EventHandler(Event& event) override;
    void RenderGui() override;
    void SetViewportSize(uint32_t width, uint32_t height) override;
    void SetState(bool is_playing) override { is_playing_ = is_playing; }

    // Getters
    std::vector<std::filesystem::path> FavDirecotries() const override;
    ImguiFont RegularFontData() const override;
    ImguiFont BoldFontData() const override;
    std::string OpenSavedScene() const override { return AM::ClientAsset("scenes/Mario_Scene.ikanScene"); }
    std::string CbpRootDir() const override { return AM::ClientAsset("scenes/"); };
    glm::vec4 GetBgColor() const override { return level_bg_; }
    std::string GameName() const override { return "Mario"; }

    static Mario& Get() { return *instance_; }

  private:
    enum Direction {Left, Right, Up, Down};

    /// This function handles the Key button event
    /// - Parameter e: Key pressed event
    bool KeyPressEvent(KeyPressedEvent& e);

    /// This function Finds the player entity "Named as Mario Player". And add the player Controller with its entity. If not created then it creates one with all the required components
    void SearchOrCreatePlayer();
    /// This function add the scripts to entites
    void AddScriptsToEntities();

    // Debug APIS
    /// This function render the rectangle when clicg drag mouse
    void SelectEntities();
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
    /// This hack moves the camera without camera controller with keys
    /// - Parameter ts: time step
    void MoveCameraDebug(Timestep ts);
    
    // ----------------------
    // Member variables
    // ----------------------
    // Mario Data
    static const uint32_t MaxTime = 100;
    glm::vec4 level_bg_ = {0.2, 0.4, 0.6, 1.0f};

    uint32_t score_ = 0;
    uint32_t world_ = 1;
    uint32_t level_ = 1;
    uint32_t coins_ = 0;
    uint32_t time_left_ = 0;
    
    float timer_ = 0;

    // Renderer Data
    bool is_playing_ = false;
    std::string font_path_ = "fonts/mario.ttf";
    TextData text_data_;

    // Viewport Data
    uint32_t viewport_width_ = Application::Get().GetWindow().GetWidth();
    uint32_t viewport_height_ = Application::Get().GetWindow().GetHeight();
    const Viewport* const viewport_;
    
    std::shared_ptr<EnttScene> scene_;
    ScenePanelManager* panel_;

    // Debug Data
    std::unordered_map<entt::entity, Entity*> selected_entities_;
    
    static Mario* instance_;
  };
  
}

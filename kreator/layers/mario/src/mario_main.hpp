//
//  mario_main.hpp
//  mario
//
//  Created by Ashish . on 02/01/23.
//

#pragma once

#include "background.hpp"
#include "player.hpp"

namespace mario {
  
  using namespace ikan;
  
  struct TextData {
    const glm::vec2 size = { 0.6f, 0.6f };
    const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

    glm::mat4 still_camera_projection;
    float score_pos_row[2];
    float score_pos_col[4];
    
    void Update(float width, float height) {
      still_camera_projection = glm::ortho( 0.0f, (float)width, 0.0f, (float)height);
      
      score_pos_row[0] = (float)height - 50;
      score_pos_row[1] = (float)height - 80;
      
      score_pos_col[0] = 50.0f;
      score_pos_col[1] = ((float)width * (1.0f / 4.0f)) + 50.0f;
      score_pos_col[2] = ((float)width * (2.0f / 4.0f)) + 50.0f;
      score_pos_col[3] = ((float)width * (3.0f / 4.0f)) + 50.0f;
    }
    
    void Render(const std::string& title, uint32_t row, uint32_t col) {
      TextRenderer::RenderText(title,
                               still_camera_projection,
                               { score_pos_col[col], score_pos_row[row], 0.3f },
                               size,
                               color);
    }
  };
  
  class MarioLayer : public Layer {
  public:
    /// Layer Default Constructor to store the name of layer
    MarioLayer();
    /// Default Destructor
    ~MarioLayer();
    
    /// This funciton initialises the layer, calls when we push layer in the application (To be called only once)
    void Attach() override;
    /// This funciton destroy the layer, calls when we pop layer from the application (To be called only once)
    void Detach() override;
    
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// NOTE Core Application is taking care to call the RenderGui API for all Layers
    void RenderGui() override;
    /// This function updates the layer data. To be called each frame from application. NOTE Core
    /// Application is taking care to call the Update(ts) API for all Layers
    /// - Parameter ts: Time step between 2 Frames
    void Update(Timestep ts) override;
    /// This function handles all the events triggered for the layer. To be whenever an event is triggered
    /// in the window. NOTE Core Application is taking care to call the EventHandler(event) API for all Layers
    /// - Parameter event: Event (Base class) intance. Dispatch event from Event Dispatcher
    void EventHandler(Event& event) override;
    
  private:
    // --------------------
    // Member functions
    // --------------------
    /// This function handles the window resize
    /// - Parameter e: window resize event
    bool OnWindowResized(WindowResizeEvent& e);
    /// This function render the scene
    /// - Parameter ts: Time step between 2 Frames
    void Render(Timestep ts);
    /// This function create the camera enity
    /// - Parameters:
    ///   - scene: active scene
    ///   - camera_entity: canmera entity reference
    void CreateCamera(EnttScene* scene, Entity& camera_entity);

    // --------------------
    // Member variables
    // --------------------
    uint32_t viewport_width_ = 0, viewport_height_ = 0;
    EnttScene mario_tile_scene_, mario_texture_scene_;
    Entity tile_camera_entity_, texture_camera_entity;
    
    // mario Data
    bool use_sprite_ = true;
    BackgroudData* background_data_;
    Player* player_;
    
    TextData text_data_;
    
    uint32_t score_ = 0;
    uint32_t world_ = 1;
    uint32_t level_ = 1;
    uint32_t coins_ = 0;
    uint32_t time_ = 100;
  
    // For Debug
#if MARIO_DEBUG
    /// This function handles the mouse button event
    /// - Parameter e: mouse button pressed event
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    
    Viewport viewport_ = Viewport({ 0.12, 0.23, 0.34, 1.0f });
    ScenePanelManager spm_;
#endif
  };
  
}

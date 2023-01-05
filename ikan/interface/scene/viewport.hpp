//
//  viewport.hpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

#include "renderer/graphics/framebuffer.hpp"

namespace ikan {

  class Entity;

  struct Viewport {
    bool focused = false;
    bool hovered = false;
    
    uint32_t width = 0.0f;
    uint32_t height = 0.0f;

    int32_t mouse_pos_x = -1;
    int32_t mouse_pos_y = -1;

    glm::vec2 bounds[2];
    ImVec2 cursor_pos;

    std::shared_ptr<FrameBuffer> framebuffer;

    int32_t hovered_entity_id_ = -1; // Entity and ID hovered by mouse position
    Entity* hovered_entity_ = nullptr; // Entity Handler to store Viewport

    DELETE_COPY_MOVE_CONSTRUCTORS(Viewport);

    // -----------------
    // Member function
    // -----------------
    Viewport(const glm::vec4& fb_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    
    /// This function updates the Mouse position inside the viewport. Should be called each frame to get mouse position precisely
    void UpdateMousePos();
    /// This function updates the porition of Viewport bound. Should be called each frame to get mouse position precisely
    void UpdateBound();
    /// This function returns true if viewport size is not same as framebuffer
    bool IsFramebufferResized();
    /// This function render the viewport information in gui
    /// - Parameter is_open: flag to show or hide the widget
    void RenderGui(bool *is_open);
  };
  
}

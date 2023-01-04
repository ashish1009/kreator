//
//  property_grid.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the wrapeer class to suppoer propert grid api

#include <imgui_internal.h>

namespace ikan {
  
  /// This class stores the Wrapper API for showing Data in Imgui window
  class PropertyGrid {
  public:
    /// This function render a hint message when hovered last imgui item
    /// - Parameters:
    ///   - description: String to be used as hint
    ///   - error: flag to render text in red
    static void HoveredMsg(const char* description,
                           bool error = false);
    /// This function render a hint icon (?) and on hovered A hint string will be shown in subwindow
    /// on hovering the mouse
    /// - Parameters:
    ///   - description: String to be used as hint
    ///   - icon_char: Icon to be printed as Help Marker (By default (?) will be rendered)
    ///   - same_line: check for same line icon or in new line
    static void HelpMarker(const char* description,
                           const char* icon_char = "?",
                           bool same_line = true);
    /// This function renderes a search box with search button
    /// - Parameters:
    ///   - value: value written in the box
    ///   - hint: hint to be shown
    static bool Search(char* value,
                       const char* hint,
                       const glm::vec2& size = { 15.0f, 15.0f });
    /// This function renders Imgae in current Imgui window
    /// - Parameters:
    ///   - textureID; Texture id of Image to be rendered
    ///   - size; size of image need to be rendered inside Imgui window (this size will be visible as texture)
    ///   - uv0; Left
    ///   - uv1; Right
    static void Image(void* textureID,
                      const glm::vec2& size,
                      const glm::vec2& uv0,
                      const glm::vec2& uv1);
    /// This funcrtion renders a Image Button with lable ID
    /// - Parameters:
    ///   - lableId: lable ID
    ///   - texId: Texture ID
    ///   - size: size of textire
    static bool ImageButton(const int32_t lable_id,
                            uint32_t texId,
                            const glm::vec2& size);
    
    /// This function renderes a image button
    /// - Parameters:
    ///   - lableId: image lable string
    ///   - texId: texture id
    ///   - size: size of image
    ///   - bgColor: background color
    ///   - padding: padding
    static bool ImageButton(const std::string& lable_id,
                            uint32_t texId,
                            const glm::vec2& size,
                            const glm::vec4& bgColor = glm::vec4(0.0f),
                            int32_t padding = 0);

    MAKE_PURE_STATIC(PropertyGrid);
  };
  
}

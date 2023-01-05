//
//  imgui_api.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the Wrapper class Imgui API

#include "imgui/imgui_layer.hpp"

namespace ikan {
  
  /// Imgui Wrapper class
  class ImguiAPI {
  public:
    // ------------
    // Docking
    // ------------
    /// This function Enables the docking in Imgui. To be called each frame
    /// before every imgui rendering;
    static void StartDcocking();
    /// This function Disable the docking in Imgui. To be called each frame
    /// after every imgui rendering;
    static void EndDcocking();

    // ------------
    // Widget
    // ------------
    /// This function renders a widgit that shows the frame rate of the application
    /// - Parameter is_open: flag to show or hide the widget
    static void Framerate(bool *is_open);
    
    // ---------------
    // Controller
    // ---------------
    /// This function changes the imgui font
    ///  - Parameters:
    ///   - default_font_path: Default font file information
    ///   - bold_font_path: bold font file information
    static void ChangeFont(const ImguiFont& default_font_path,
                           const ImguiFont& bold_font_path);

    MAKE_PURE_STATIC(ImguiAPI);
  };
  
}

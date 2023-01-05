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
    
    MAKE_PURE_STATIC(ImguiAPI);
  };
  
}

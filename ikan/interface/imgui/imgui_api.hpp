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
    /// This function renders a widgit that shows the frame rate of the application
    static void Framerate();
    MAKE_PURE_STATIC(ImguiAPI);
  };
  
}

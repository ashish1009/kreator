//
//  glfw_window.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

#include "core/utils/time_step.hpp"
#include "core/events/event.hpp"

// This file includes the abstract interface for Window class

namespace ikan {
  
  /// This class is the abstract class for creating Window instance. This class defines all the API to
  /// be used by core or client. To Create instance of window Call the static API ::Create with
  /// Operating system type and specificaion of windows
  class Window {
  public:
    /// This structure holds the specification for window. Specification provided by window.
    struct Specification {
      std::string title = "Default Window";
      uint32_t width = 1600, height = 900;
      
      bool v_sync = true; // Set flag to make window v syncronised
      bool fullscreen = false; // Set the window as full screen
      bool decorated = true; // if false : Hides the Title bar and close,
                             // minimize and Maximize icon also Disable resizable
      
      void Log();

      Specification();
      ~Specification();
      
      DEFINE_COPY_MOVE_CONSTRUCTORS(Specification);
    };
    
    /// Default virtual destructor
    virtual ~Window() noexcept = default;
  };
  
}

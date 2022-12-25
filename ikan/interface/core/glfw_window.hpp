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
    
    // ----------------------
    // Fundamentals
    // ----------------------
    /// This function updates the window for each frame. This function to be called by application on
    /// each frame. It swap the context buffers. Also update the time of each frame
    virtual void Update() = 0;
    /// This funtion shut down the current window and destroy the GLFW Context
    virtual void Shutdown() = 0;
    /// This function updates the event callback function and register it to the window instance
    /// NOTE: EventCallbackFn to be called every time any even triggered to window
    /// - Parameter event_callback_fn: function pointer to be registered in the window
    virtual void SetEventFunction(const EventCallbackFn& event_callback_fn) = 0;
    /// This function maximise the window to its full length based on the current monitor. Call when
    /// Window need to be maximised
    virtual void Maximize() = 0;
    /// This function places the window at the center of the screen
    virtual void CenterWindow() = 0;
    
    // -------------
    // Setters
    // -------------
    /// This funtions changes the VSync flag by argiument flag
    /// - Parameter vSync: new flag to be set as v sync
    virtual void SetVSync(bool vSync) = 0;
    /// This function makes Window resizable if 'resizable' is true else Window can not be resized
    /// after calling this API
    /// - Parameter resizable: new flag as resizable
    virtual void SetResizable(bool resizable) const = 0;
    /// This function chnages the title of the window as 'new_title'
    /// - Parameter new_title: new title of the window
    virtual void SetTitle(const std::string& new_title) = 0;
    
    // --------------------
    // Getters
    // --------------------
    /// This function returns is window is v synched
    virtual bool IsVSync() const = 0;
    /// This function returns is window is decoarated. When decorated then removes titlebar from the
    /// window
    virtual bool IsDecorated() const = 0;
    /// This function returns is the window is full screen
    virtual bool IsFullscreen() const = 0;
    /// This function returns the width of window
    virtual uint32_t GetWidth() const = 0;
    /// This function returns the height of window
    virtual uint32_t GetHeight() const = 0;
    /// This function returns the title of the window
    virtual std::string GetTitle() const = 0;
    /// This function returns the current time step of this frame
    virtual Timestep GetTimestep() const = 0;
    /// This function returns the native window pointer (void*)
    [[nodiscard]] virtual void* GetNativeWindow() const = 0;
    
    /// This function create instance of to create a window based on the Operating system supported
    /// by the sytem / Engine. This is resposible to create the GLFW Window instance for the OS and
    /// Create the Renderer Context for the suppoorted Renderer API.
    /// NOTE: Set the renderer API befiore creating the Widnow, as it is needed to creaste the Contexrt
    /// - Parameters:
    ///   - os: Current operating system
    ///   - specificaiton: window specification
    static std::unique_ptr<Window> Create(OperatingSystem os,
                                          const Specification& spec);
  };
  
}

//
//  mac_os_window.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the implementation of a class to create a window for MAC OS

#include "core/glfw_window.hpp"
#include "renderer/graphics/renderer_context.hpp"

namespace ikan {
  
  /// This class is responsble for creating Window for MAC OS using GLFW
  class MacWindow : public Window {
  public:
    /// Mac window Data
    struct Data {
      Window::Specification specification;
      EventCallbackFn event_callback_function;
      
      Data() = default;
      
      DELETE_COPY_MOVE_CONSTRUCTORS(Data);
    };

    // -------------------------------------
    // Constructors and Destructors
    // -------------------------------------
    /// This constructuor creates an GLFW window for MAC OS. Register the data
    /// in window set back pointer
    /// - parameter specification: window specification
    MacWindow(const Window::Specification& specification);
    /// Default destructor. Destroy the GLFW window
    ~MacWindow();
    
    // ---------------
    // Fundamentals
    // ---------------
    /// This function updates the window for each frame. This function to be called by application on each
    /// frame. It swap the context buffers. Also update the time of each frame
    void Update() override;
    /// This funtion shut down the current window and destroy the GLFW Context
    void Shutdown() override;
    /// This function Updates the callback funtion pointer to the mac_window_data_. This function pointer
    /// to be triggered for every event interrupt
    /// - Parameter event_callback_fn: function pointer to be registered
    void SetEventFunction(const EventCallbackFn& event_callback_fn) override;
    /// This function maximise the window to its full length based on the current monitor. Call when Window
    /// need to be maximised
    void Maximize() override;
    /// This function places the window at the center of the screen
    void CenterWindow() override;
    
    // ---------
    // Setters
    // ---------
    /// This funtions changes the VSync flag by argiument flag
    /// - Parameter vSync: new flag to be set as v sync
    void SetVSync(bool vSync) override;
    /// This function makes Window resizable if 'resizable' is true else Window can not be resized after
    /// calling this API
    /// - Parameter resizable: new flag as resizable
    void SetResizable(bool resizable) const override;
    /// This function chnages the title of the window as 'new_title'
    /// - Parameter new_title: new title of the window
    void SetTitle(const std::string& new_title) override;
    
    // ---------
    // Getters
    // ---------
    /// This function returns is window is v synched
    bool IsVSync() const override;
    /// This function returns is window is decoarated. When decorated then removes titlebar from the
    /// window
    bool IsDecorated() const override;
    /// This function returns is the window is full screen
    bool IsFullscreen() const override;
    /// This function returns the width of window
    uint32_t GetWidth() const override;
    /// This function returns the height of window
    uint32_t GetHeight() const override;
    /// This function returns the title of the window
    std::string GetTitle() const override;
    /// This function returns the current time step of this frame
    Timestep GetTimestep() const override;
    /// This function returns the native window pointer (void*)
    [[nodiscard]] void* GetNativeWindow() const override;
    
  private:
    // ----------------
    // Member methods
    // ----------------
    /// Set the callback to window data
    void SetEventCallbacks();

    // ----------------
    // Member variables
    // ----------------
    GLFWwindow* window_;
    Data mac_window_data_;
    std::unique_ptr<RendererContext> renderer_context_;
    Timestep time_step_;
    float last_frame_time_ = 0.0f;
  };
  
}

//
//  renderer_context.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the interface for creating Graphics Renderer Context in
// Corresponding window

#include <GLFW/glfw3.h>

namespace ikan {
  
  /// This class is the Interface for Graphics Renderer Context. Instance to be created based on the
  /// current API Supported
  class RendererContext {
  public:
    // -------------
    // Destrcutor
    // -------------
    /// This is default virtual constructor
    virtual ~RendererContext() noexcept = default;
    // -------------
    // Fundamentals
    // -------------
    /// This function initialises the Graphics renderer context as per supported API
    virtual void Init() = 0;
    /// This function swaps the graphics buffer. This should be called each frame
    virtual void SwapBuffers() = 0;
    
    // -----------------
    // Static Function
    // -----------------
    /// This function creates the instance of Renderer Context based in the API configured in the
    /// Renrerer Class.
    /// - Parameter window: GLFW Window pointer
    static std::unique_ptr<RendererContext> Create(GLFWwindow* window);
  };
  
} // namespace ikan

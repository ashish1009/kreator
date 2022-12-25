//
//  open_gl_renderer_context.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the implementation class for Open GL renderer context

#include "renderer/graphics/renderer_context.hpp"

namespace ikan {
  
  /// This is the Implementation class of Open GL Renderer Graphics context
  class OpenGLRendererContext : public RendererContext {
  public:
    // -----------------------------------
    // Constructors and Destructor
    // -----------------------------------
    /// This constructor creates the Open GL Context using GLFW 'window' pointer
    /// - Parameter window: GLFW window for which context need to be created
    OpenGLRendererContext(GLFWwindow* window);
    /// This destructor destroy the context mapped to window
    virtual ~OpenGLRendererContext() noexcept;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLRendererContext);
    
    // --------------
    // Fundamentals
    // --------------
    /// This function initialize the the Renderer context
    void Init() override;
    /// This function swaps the renderer buffers, Should be called each frame
    void SwapBuffers() override;
    
  private:
    GLFWwindow* window_;
  };
  
}

//
//  open_gl_renderer_context.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include <glad/glad.h>

#include "open_gl_renderer_context.hpp"
#include "renderer/utils/renderer.hpp"

namespace ikan {
  
  OpenGLRendererContext::OpenGLRendererContext(GLFWwindow* window)
  : window_(window) {
    IK_CORE_ASSERT(window, "Window pointer is NULL !!!");
    IK_CORE_INFO("Creating Open GL Renderer Context ... ");
    Init();
  }
  
  OpenGLRendererContext::~OpenGLRendererContext() noexcept {
    IK_CORE_WARN("Destroying Open GL Renderer Context !!! ");
  }
  
  void OpenGLRendererContext::Init() {
    // ------------------------------------------------------------------------
    // Create Renderer Context for GLFW Window
    // NOTE: This function makes the OpenGL or OpenGL ES context of the
    //       specified window on the calling calling thread. A context must only
    //       be made current on a single thread at a time and each thread can
    //       have only a single context at a time.
    // ------------------------------------------------------------------------
    glfwMakeContextCurrent(window_);
    
    // ------------------------------------------------------------------------
    // We pass GLAD the function to load the address of the OpenGL function
    // pointers which is OS-specific. GLFW gives us glfwGetProcAddress that
    // defines the correct function based on which OS we're compiling for
    // ------------------------------------------------------------------------
    IK_CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
                   "Can not initialize the Glad");
    
    auto& caps = Renderer::Capabilities::Get();
    
    caps.vendor   = (const char*)glGetString(GL_VENDOR);
    caps.renderer = (const char*)glGetString(GL_RENDERER);
    caps.version  = (const char*)glGetString(GL_VERSION);
    
    caps.Log();
  }
  
  void OpenGLRendererContext::SwapBuffers() {
    glfwSwapBuffers(window_);
  }
  
} // namespace ikan

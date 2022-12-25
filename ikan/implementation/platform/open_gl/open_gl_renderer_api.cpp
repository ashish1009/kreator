//
//  open_gl_renderer_api.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include <glad/glad.h>

#include "open_gl_renderer_api.hpp"
#include "renderer/utils/renderer.hpp"

namespace ikan {
  
  OpenGLRendererAPI::OpenGLRendererAPI() {
    IK_CORE_TRACE("Creating Open GL Renderer API ...");
    Init();
  }
  
  OpenGLRendererAPI::~OpenGLRendererAPI() noexcept {
    IK_CORE_WARN("Destroying Open GL Renderer API !!!");
    Shutdown();
  }
  
  void OpenGLRendererAPI::Init() const {
    IK_CORE_INFO("Initializeing Open GL Renderer API");
        
    auto& caps = Renderer::Capabilities::Get();
    
    caps.vendor   = (const char*)glGetString(GL_VENDOR);
    caps.renderer = (const char*)glGetString(GL_RENDERER);
    caps.version  = (const char*)glGetString(GL_VERSION);
  }
  
  void OpenGLRendererAPI::Shutdown() const {
    IK_CORE_WARN("Shutting down Open GL Renderer API");
  }
  
}

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
  
  /// Convert Renderer GL Func to Open GL Func
  /// - Parameter func: Renderer Func enum
  static GLint FucToGlFunc(RendererAPI::GlFunc func) {
    switch (func) {
      case RendererAPI::GlFunc::Always :
        return GL_ALWAYS;
      case RendererAPI::GlFunc::Never :
        return GL_NEVER;
      case RendererAPI::GlFunc::Less :
        return GL_LESS;
      case RendererAPI::GlFunc::Equal :
        return GL_EQUAL;
      case RendererAPI::GlFunc::LEqual :
        return GL_LEQUAL;
      case RendererAPI::GlFunc::Greater :
        return GL_GREATER;
      case RendererAPI::GlFunc::LGreater :
        return GL_GEQUAL;
      case RendererAPI::GlFunc::NotEqual :
        return GL_NOTEQUAL;
      default:
        IK_CORE_ASSERT(false, "Invalid Type");
    }
  }
  
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
    
    // API for Text enable 
    IK_CORE_INFO("  Asignment           | {0} (Unpacked)", true);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Multi Sample
    IK_CORE_INFO("  Multi Sample Feild  | {0}", true);
    glEnable(GL_MULTISAMPLE);
    
    // Blending Teting
    IK_CORE_INFO("  Blending Test Feild | {0}", true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Depth Testing
    IK_CORE_INFO("  Depth Test Feild    | {0}", true);
    glEnable(GL_DEPTH_TEST);
    
    auto& caps = Renderer::Capabilities::Get();
    
    caps.vendor   = (const char*)glGetString(GL_VENDOR);
    caps.renderer = (const char*)glGetString(GL_RENDERER);
    caps.version  = (const char*)glGetString(GL_VERSION);
  }
  
  void OpenGLRendererAPI::Shutdown() const {
    IK_CORE_WARN("Shutting down Open GL Renderer API");
  }
  
  void OpenGLRendererAPI::Depth(bool state) const {
    IK_CORE_INFO("  Depth Test Feild    | {0}", state);
    if (state)
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);
  }
  
  void OpenGLRendererAPI::Blend(bool state) const {
    IK_CORE_INFO("  Blending Test Feild | {0}", state);
    if (state) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
      glDisable(GL_BLEND);
    }
  }
  
  void OpenGLRendererAPI::MultiSample(bool state) const {
    IK_CORE_INFO("  Multi Sample Feild  | {0}", state);
    if (state)
      glEnable(GL_MULTISAMPLE);
    else
      glDisable(GL_MULTISAMPLE);
  }
  
  void OpenGLRendererAPI::DepthFunc(GlFunc func) const {
    glDepthFunc(FucToGlFunc(func));
  }
  
  void OpenGLRendererAPI::BeginWireframe() const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  
  void OpenGLRendererAPI::EndWireframe() const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  
  void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) const {
    glClearColor(color.r, color.g, color.b, color.a);
  }
  
  void OpenGLRendererAPI::ClearBits() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
  void OpenGLRendererAPI::ClearDepthBit() const {
    glClear(GL_DEPTH_BUFFER_BIT);
  }
  
  void OpenGLRendererAPI::ClearStencilBit() const {
    glClear(GL_STENCIL_BUFFER_BIT);
  }
  
  void OpenGLRendererAPI::ClearColorBit() const {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  
  void OpenGLRendererAPI::SetViewport(uint32_t width, uint32_t height) const {
    glViewport(0, 0, width, height);
  }
  
  void OpenGLRendererAPI::GetEntityIdFromPixels(int32_t mx,
                                                int32_t my,
                                                uint32_t pixel_id_index,
                                                int32_t& pixel_data) const {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + pixel_id_index);
    glReadPixels(mx, my, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
  }
}

//
//  renderer_context.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "renderer_context.hpp"

#include "renderer/utils/renderer.hpp"
#include "platform/open_gl/open_gl_renderer_context.hpp"

namespace ikan {
  
  std::unique_ptr <RendererContext> RendererContext::Create(GLFWwindow *window) {
    switch(Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_unique<OpenGLRendererContext>(window);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API which is not supported by ikan engine" );
    }
  }
  
}

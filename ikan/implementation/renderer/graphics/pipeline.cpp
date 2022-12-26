//
//  pipeline.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "pipeline.hpp"
#include "platform/open_gl/open_gl_pipeline.hpp"

namespace ikan {
  
  std::shared_ptr<Pipeline> Pipeline::Create() {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLPipeline>();
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
}

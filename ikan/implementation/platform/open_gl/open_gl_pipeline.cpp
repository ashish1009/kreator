//
//  pipeline.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "open_gl_pipeline.hpp"
#include "platform/open_gl/open_gl_renderer_id_manager.hpp"
#include <glad/glad.h>

namespace ikan {
  
  OpenGLPipeline::OpenGLPipeline() {
    IDManager::GetPipelineId(renderer_id_);
    glBindVertexArray(renderer_id_);
    
    IK_CORE_DEBUG("Creating Open GL Pipeline ...");
    IK_CORE_DEBUG("  Renderer ID | {0}", renderer_id_);
  }
  
  OpenGLPipeline::~OpenGLPipeline() noexcept {
    IK_CORE_WARN("Destroying Open GL Pipeline !!!");
    IK_CORE_WARN("  Renderer ID | {0}", renderer_id_);

    IDManager::RemovePipelineId(renderer_id_);
  }
  
}

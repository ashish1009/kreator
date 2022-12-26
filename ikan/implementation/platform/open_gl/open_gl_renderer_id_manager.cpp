//
//  open_gl_renderer_id_manager.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "open_gl_renderer_id_manager.hpp"
#include <glad/glad.h>

namespace ikan {
  
  void OpenGlRendererIdManager::GetShaderId(RendererID &renderer_id) {
    renderer_id = glCreateProgram();
  }
  void OpenGlRendererIdManager::GetBufferId(RendererID &renderer_id) {
  }
  void OpenGlRendererIdManager::GetPipelineId(RendererID &renderer_id) {
  }
  void OpenGlRendererIdManager::GetTextureId(RendererID &renderer_id) {
  }
  void OpenGlRendererIdManager::GetFramebufferId(RendererID &renderer_id) {
  }
  
}

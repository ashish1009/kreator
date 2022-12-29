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
    glGenBuffers(1, &renderer_id);
  }
  void OpenGlRendererIdManager::GetPipelineId(RendererID &renderer_id) {
    glGenVertexArrays(1, &renderer_id);
  }
  void OpenGlRendererIdManager::GetTextureId(RendererID &renderer_id) {
    glGenTextures(1, &renderer_id);
  }
  void OpenGlRendererIdManager::GetFramebufferId(RendererID &renderer_id) {
  }

  void OpenGlRendererIdManager::RemoveShaderId(RendererID &renderer_id) {
    glDeleteProgram(renderer_id);
  }
  void OpenGlRendererIdManager::RemoveBufferId(RendererID &renderer_id) {
    glDeleteBuffers(1, &renderer_id);
  }
  void OpenGlRendererIdManager::RemovePipelineId(RendererID &renderer_id) {
    glDeleteVertexArrays(1, &renderer_id);
  }
  void OpenGlRendererIdManager::RemoveTextureId(RendererID &renderer_id) {
    glDeleteTextures(1, &renderer_id);
  }
  void OpenGlRendererIdManager::RemoveFramebufferId(RendererID &renderer_id) {
    glDeleteFramebuffers(1, &renderer_id);
  }

}

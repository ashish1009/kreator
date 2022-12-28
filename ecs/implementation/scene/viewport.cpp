//
//  viewport.cpp
//  ecs
//
//  Created by Ashish . on 28/12/22.
//

#include "viewport.hpp"

namespace ecs {
  
  Viewport::Viewport(const glm::vec4& fb_color) {
    FrameBuffer::Specification fb_spec;
    fb_spec.color = fb_color;
    fb_spec.attachments = {
      FrameBuffer::Attachments::TextureFormat::RGBA8,
      FrameBuffer::Attachments::TextureFormat::Depth24Stencil,
    };
    
    framebuffer = FrameBuffer::Create(fb_spec);
  }
  
}

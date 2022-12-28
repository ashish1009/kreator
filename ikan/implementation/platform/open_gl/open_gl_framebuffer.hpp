//
//  open_gl_framebuffer.hpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

// This file includes the implementation of Renderer Framebuffer for Open GL

#include "renderer/graphics/framebuffer.hpp"

namespace ikan {
  
  /// Implementation to store Open GL Framebuffer
  class OpenGLFrameBuffer : public FrameBuffer {
  public:
    // ----------------------------------
    // Constructors and Destructor
    // ----------------------------------
    /// This is the consturctor of frame buffer the create the buffer with the specification
    /// - parameter spec: Framebuffer current specification
    OpenGLFrameBuffer(const Specification& spec);
    /// This is the default frame buffer destructor that delete the buffer
    ~OpenGLFrameBuffer();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLFrameBuffer);
    
    // ------------------
    // Member variables
    // ------------------
    RendererID renderer_id_ = 0;
    Specification specification_;
    
    // Stores the attachments of Framebuffer
    std::vector<Attachments::TextureFormat> color_specifications_;
    Attachments::TextureFormat depth_specification_ = Attachments::TextureFormat::None;
  };
  
}

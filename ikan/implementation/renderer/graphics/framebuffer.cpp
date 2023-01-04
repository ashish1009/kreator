//
//  framebuffer.cpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#include "framebuffer.hpp"
#include "platform/open_gl/open_gl_framebuffer.hpp"

namespace ikan {
  
  // --------------------------------------------------------------------------
  // FrameBuffer Attachments
  // --------------------------------------------------------------------------
  FrameBuffer::Attachments::Attachments(const FrameBuffer::Attachments& other)
  : texture_formats(other.texture_formats) {
    IK_CORE_TRACE("Frame Buffer", "Framebuffer Attachment Copied");
  }
  
  FrameBuffer::Attachments::Attachments(FrameBuffer::Attachments&& other)
  : texture_formats(other.texture_formats) {
    other.texture_formats.clear();
    IK_CORE_TRACE("Frame Buffer", "Framebuffer Attachment Moved");
  }
  
  FrameBuffer::Attachments&
  FrameBuffer::Attachments::operator=(const FrameBuffer::Attachments& other) {
    IK_CORE_TRACE("Frame Buffer", "Copying (= operator) FrameBuffer::Attachment  ");
    texture_formats = other.texture_formats;
    return *this;
  }
  
  FrameBuffer::Attachments&
  FrameBuffer::Attachments::operator=(FrameBuffer::Attachments&& other) {
    IK_CORE_TRACE("Frame Buffer", "Moving (= operator) FrameBuffer::Attachment  ");
    texture_formats = other.texture_formats;
    other.texture_formats.clear();
    return *this;
  }
  
  FrameBuffer::Attachments::Attachments(std::initializer_list<Attachments::TextureFormat> attachments)
  : texture_formats(attachments) {
  }
  
  // --------------------------------------------------------------------------
  // Frame Buffer Specification
  // --------------------------------------------------------------------------
  FrameBuffer::Specification::Specification() {
    IK_CORE_TRACE("Frame Buffer", "Creating FrameBuffer Specification ... ");
  }
    
  FrameBuffer::Specification::~Specification() {
    IK_CORE_WARN("Frame Buffer", "Destroying Frame Buffer Specification !!!");
  }
  
  FrameBuffer::Specification::Specification(const FrameBuffer::Specification& other)
  : width(other.width), height(other.height), color(other.color),
  attachments(other.attachments) {
    IK_CORE_TRACE("Frame Buffer", "Framebuffer Specification Copied");
  }
  
  FrameBuffer::Specification::Specification(FrameBuffer::Specification&& other)
  : width(other.width), height(other.height), color(other.color),
  attachments(std::move(other.attachments)) {
    IK_CORE_TRACE("Frame Buffer", "Framebuffer Specification Moved");
  }
  
  FrameBuffer::Specification&
  FrameBuffer::Specification::operator=(const FrameBuffer::Specification& other) {
    IK_CORE_TRACE("Frame Buffer", "Copying (= operator) FrameBuffer::Attachment  ");
    width = other.width;
    height = other.height;
    color = other.color;
    attachments = other.attachments;
    return *this;
  }
  
  FrameBuffer::Specification&
  FrameBuffer::Specification::operator=(FrameBuffer::Specification&& other) {
    IK_CORE_TRACE("Frame Buffer", "Moving (= operator) FrameBuffer::Attachment  ");
    width = other.width;
    height = other.height;
    color = other.color;
    attachments = std::move(other.attachments);
    return *this;
  }
  
  // --------------------------------------------------------------------------
  // FrameBuffer API
  // --------------------------------------------------------------------------
  std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FrameBuffer::Specification& spec) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLFrameBuffer>(spec);
      case Renderer::Api::None:
      default: IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
    
  }

}

// No matching conversion for functional-style cast from 'const FrameBuffer::Specification' to 'std::shared_ptr<OpenGLFrameBuffer>'

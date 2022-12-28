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
  : TextureFormats(other.TextureFormats) {
    IK_CORE_TRACE("Framebuffer Attachment Copied");
  }
  
  FrameBuffer::Attachments::Attachments(FrameBuffer::Attachments&& other)
  : TextureFormats(other.TextureFormats) {
    other.TextureFormats.clear();
    IK_CORE_TRACE("Framebuffer Attachment Moved");
  }
  
  FrameBuffer::Attachments&
  FrameBuffer::Attachments::operator=(const FrameBuffer::Attachments& other) {
    IK_CORE_TRACE("Copying (= operator) FrameBuffer::Attachment  ");
    TextureFormats = other.TextureFormats;
    return *this;
  }
  
  FrameBuffer::Attachments&
  FrameBuffer::Attachments::operator=(FrameBuffer::Attachments&& other) {
    IK_CORE_TRACE("Moving (= operator) FrameBuffer::Attachment  ");
    TextureFormats = other.TextureFormats;
    other.TextureFormats.clear();
    return *this;
  }
  
  FrameBuffer::Attachments::Attachments(std::initializer_list<Attachments::TextureFormat> attachments)
  : TextureFormats(attachments) {
  }
  
  // --------------------------------------------------------------------------
  // Frame Buffer Specification
  // --------------------------------------------------------------------------
  FrameBuffer::Specification::Specification() {
    IK_CORE_DEBUG("Creating FrameBuffer Specification ... ");
  }
    
  FrameBuffer::Specification::~Specification() {
    IK_CORE_DEBUG("Destroying Frame Buffer Specification !!!");
  }
  
  FrameBuffer::Specification::Specification(const FrameBuffer::Specification& other)
  : width(other.width), height(other.height), color(other.color),
  attachments(other.attachments) {
    IK_CORE_TRACE("Framebuffer Specification Copied");
  }
  
  FrameBuffer::Specification::Specification(FrameBuffer::Specification&& other)
  : width(other.width), height(other.height), color(other.color),
  attachments(std::move(other.attachments)) {
    IK_CORE_TRACE("Framebuffer Specification Moved");
  }
  
  FrameBuffer::Specification&
  FrameBuffer::Specification::operator=(const FrameBuffer::Specification& other) {
    IK_CORE_TRACE("Copying (= operator) FrameBuffer::Attachment  ");
    width = other.width;
    height = other.height;
    color = other.color;
    attachments = other.attachments;
    return *this;
  }
  
  FrameBuffer::Specification&
  FrameBuffer::Specification::operator=(FrameBuffer::Specification&& other) {
    IK_CORE_TRACE("Moving (= operator) FrameBuffer::Attachment  ");
    width = other.width;
    height = other.height;
    color = other.color;
    attachments = std::move(other.attachments);
    return *this;
  }
  
}

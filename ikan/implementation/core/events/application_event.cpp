//
//  application_event.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "application_event.hpp"

namespace ikan {
  
  WindowResizeEvent::WindowResizeEvent(uint32_t width, uint32_t height)
  : width_(width), height_(height) {}
  
  uint32_t WindowResizeEvent::GetWidth() const {
    return width_;
  }
  
  uint32_t WindowResizeEvent::GetHeight() const {
    return height_;
  }
  
  void WindowResizeEvent::Print() const {
    IK_CORE_TRACE("Window Resize Event tiggered ");
    IK_CORE_TRACE("  Height | {0}", height_);
    IK_CORE_TRACE("  Width  | {0}", width_);
  }

  void WindowCloseEvent::Print() const {
    IK_CORE_TRACE("Window Close Event triggered ");
  }
  
} //namespace ikan

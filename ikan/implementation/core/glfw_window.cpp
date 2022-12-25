//
//  glfw_window.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "glfw_window.hpp"

namespace ikan {
 
  // ----------------------------
  // Window Specification
  // ----------------------------
  Window::Specification::Specification() {
    IK_CORE_TRACE("Creating Window Specification ... ");
  }
  
  Window::Specification::~Specification() {
    IK_CORE_WARN("Destroying Window Specification !!! ");
  }
  
  Window::Specification::Specification(const Window::Specification& other)
  : title(other.title), width(other.width), height(other.height),
  v_sync(other.v_sync), fullscreen(other.fullscreen),
  decorated(other.decorated) {
    IK_CORE_TRACE("Copying Window Specification ...");
  }
  
  Window::Specification::Specification(Window::Specification&& other)
  : title(other.title), width(other.width), height(other.height),
  v_sync(other.v_sync), fullscreen(other.fullscreen),
  decorated(other.decorated) {
    IK_CORE_TRACE("Moving Window Specification ...");
  }
  
  Window::Specification&
  Window::Specification::operator=(const Window::Specification& other) {
    IK_CORE_TRACE("Copying Window Specification using operator = ...");
    
    title = other.title;
    width = other.width;
    height = other.height;
    v_sync = other.v_sync;
    fullscreen = other.fullscreen;
    decorated = other.decorated;
    
    return *this;
  }
  
  Window::Specification&
  Window::Specification::operator =(Window::Specification&& other) {
    IK_CORE_TRACE("Moving Window Specification using operator = ...");
    
    title = other.title;
    width = other.width;
    height = other.height;
    v_sync = other.v_sync;
    fullscreen = other.fullscreen;
    decorated = other.decorated;
    
    return *this;
  }

  void Window::Specification::Log() {
    IK_CORE_INFO("  Window Property | Value ");
    IK_CORE_INFO("  ------------------------");
    IK_CORE_INFO("  Title           | {0}", title);
    IK_CORE_INFO("  Width           | {0}", width);
    IK_CORE_INFO("  Height          | {0}", height);
    IK_CORE_INFO("  VSync           | {0}", v_sync);
    IK_CORE_INFO("  Decorated       | {0}", decorated);
    IK_CORE_INFO("  FullScreen      | {0}", fullscreen);
  }
  
}

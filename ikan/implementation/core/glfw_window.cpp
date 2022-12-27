//
//  glfw_window.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "glfw_window.hpp"
#include "platform/mac_os/mac_os_window.hpp"

namespace ikan {
  
  // ----------------------------
  // Window API
  // ----------------------------
  std::unique_ptr<Window> Window::Create(OperatingSystem os,
                                         const Specification& specificaiton) {
    switch (os) {
      case OperatingSystem::Mac:
        return std::make_unique<MacWindow>(std::move(specificaiton));
      case OperatingSystem::None:
      default:
        IK_CORE_ASSERT(false, "Provided OS Not Suported !!!");
    }
  }
 
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
    IK_CORE_INFO("  FullScreen      | {0}", fullscreen);
    if (decorated)
      IK_CORE_INFO("  Decorated       | {0} [shows the title bar and buttons like close, minimise and maxinmize]", decorated);
    else
      IK_CORE_INFO("  Decorated       | {0} [hides the title bar and buttons like close, minimise and maxinmize]", decorated);
  }
  
}

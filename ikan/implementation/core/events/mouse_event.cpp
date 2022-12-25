//
//  mouse_event.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "mouse_event.hpp"

namespace ikan {
  
  MouseMovedEvent::MouseMovedEvent(float x, float y)
  : mouse_x_(x), mouse_y_(y) {}
  
  float MouseMovedEvent::GetX() const { return mouse_x_; }
  float MouseMovedEvent::GetY() const { return mouse_y_; }
  
  void MouseMovedEvent::Print() const {
      IK_CORE_TRACE("Mouse Move Event tiggered ");
      IK_CORE_TRACE("  X | {0}", mouse_x_);
      IK_CORE_TRACE("  Y | {0}", mouse_y_);
  }
  
  MouseScrolledEvent::MouseScrolledEvent(float x_offset, float y_offset)
  : x_offset_(x_offset), y_offset_(y_offset) {}
  
  /// return x Offset of Scrolling
  float MouseScrolledEvent::GetXOffset() const { return x_offset_; }
  /// return y Offset of Scrolling
  float MouseScrolledEvent::GetYOffset() const { return y_offset_; }
  
  void MouseScrolledEvent::Print() const {
    IK_CORE_TRACE("Mouse Scroll Event tiggered ");
    IK_CORE_TRACE("  X Offset | {0}", x_offset_);
    IK_CORE_TRACE("  Y Offset | {0}", y_offset_);
  }
  
  MouseButtonEvent::MouseButtonEvent(MouseButton button) : button_(button) {}
  
  MouseButton MouseButtonEvent::GetMouseButton() const { return button_; }
  
  MouseButtonPressedEvent::MouseButtonPressedEvent(MouseButton button)
  : MouseButtonEvent(button) {}
  
  void MouseButtonPressedEvent::Print() const {
    IK_CORE_TRACE("Mosue Buttom Press Event tiggered ");
    IK_CORE_TRACE("  Button code | {0}", (uint32_t)button_);
  }

  MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseButton button)
  : MouseButtonEvent(button) {}
  
  void MouseButtonReleasedEvent::Print() const {
    IK_CORE_TRACE("Mouse Button Release Event tiggered ");
    IK_CORE_TRACE("  Button code | {0}", (uint32_t)button_);
  }
  
} // namespace ikan

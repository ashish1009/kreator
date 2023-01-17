//
//  mouse_event.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes classes to handle the mouse events.

#include "core/events/event.hpp"
#include "core/mouse_codes.h"

namespace ikan {
  
  /// Mouse base Event class
  class MouseMovedEvent : public Event {
  public:
    /// Default virtual destructor
    virtual ~MouseMovedEvent() = default;
    
    /// Mouse moved event
    /// - Parameters:
    ///   - x: X position of Mouse
    ///   - y: Y position of Mouse
    MouseMovedEvent(float x, float y);
    
    /// This function returns the X position of mouse
    float GetX() const;
    /// This function returns the Y position of mouse
    float GetY() const;
    
    /// This function prints the event detail
    void Print() const override;
    
    EVENT_CLASS_TYPE(MouseMoved);
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    
  private:
    float mouse_x_, mouse_y_;
  };
  
  /// Mouse scrol event
  class MouseScrolledEvent : public Event {
  public:
    /// Default virtual destructor
    virtual ~MouseScrolledEvent() = default;
    
    /// Mosue Scroll Event
    /// - Parameters:
    ///   - xOffset: x Offset of Scrolling
    ///   - yOffset: y Offset of Scrolling
    MouseScrolledEvent(float x_offset, float y_offset);
    
    /// This function returns x Offset of Scrolling
    float GetXOffset() const;
    /// This function returns y Offset of Scrolling
    float GetYOffset() const;
    
    /// This function prints the event detail
    void Print() const override;
    
    EVENT_CLASS_TYPE(MouseScrolled);
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    
  private:
    float x_offset_, y_offset_;
  };
  
  /// Mouse button base calss
  class MouseButtonEvent : public Event {
  public:
    /// Default virtual destructor
    virtual ~MouseButtonEvent() = default;
    
    /// This function returns the Mouse button pressed
    MouseButton GetMouseButton() const;
    
    EVENT_CLASS_CATEGORY(EventCategoryMouse |
                         EventCategoryInput |
                         EventCategoryMouseButton)
    
  protected:
    MouseButtonEvent(MouseButton button);
    
    MouseButton button_;
  };
  
  /// Mouse button pressed
  class MouseButtonPressedEvent : public MouseButtonEvent {
  public:
    /// Default virtual destructor
    virtual ~MouseButtonPressedEvent() = default;
    
    /// Mouse button pressed Event
    /// - Parameter button: Mouse button pressed
    MouseButtonPressedEvent(MouseButton button);
    
    /// This function prints the event detail
    void Print() const override;
    
    EVENT_CLASS_TYPE(MouseButtonPressed);
  };
  
  /// Mouse button released
  class MouseButtonReleasedEvent : public MouseButtonEvent {
  public:
    /// Default virtual destructor
    virtual ~MouseButtonReleasedEvent() = default;
    
    /// Mouse button release ebent
    /// - Parameter button: Mouse button released
    MouseButtonReleasedEvent(MouseButton button);
    
    /// This function prints the event detail
    void Print() const override;
    
    EVENT_CLASS_TYPE(MouseButtonReleased);
  };
  
} // namespace ikan

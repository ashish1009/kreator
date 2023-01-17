//
//  application_event.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

#include "core/events/event.hpp"

namespace ikan {
  
  /// This class handles the event to resize the window
  class WindowResizeEvent : public Event {
  public:
    /// Default Destructor
    virtual ~WindowResizeEvent() = default;
    
    /// Constructs the event that set the new size of window
    /// - Parameter - Width of new window:
    /// - Parameter - Height of new window:
    WindowResizeEvent(uint32_t width, uint32_t height);
    /// This function returns the width of window
    uint32_t GetWidth() const;
    /// This function returns the height of window
    uint32_t GetHeight() const;
    
    /// This function prints the event detail
    void Print() const override;

    EVENT_CLASS_TYPE(WindowResize);
    EVENT_CLASS_CATEGORY(EventCategoryApplication);

  private:
    uint32_t width_, height_;
  };
  
  /// Window Close event
  class WindowCloseEvent : public Event {
  public:
    /// Default Destructor
    virtual ~WindowCloseEvent() = default;
    
    /// This function prints the event detail
    void Print() const override;
      
    EVENT_CLASS_TYPE(WindowClose);
    EVENT_CLASS_CATEGORY(EventCategoryApplication);
  };
  
} 

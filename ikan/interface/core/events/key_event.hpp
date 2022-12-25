//
//  key_event.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes classes to handle the key board events.

#include "core/events/event.hpp"
#include "core/key_codes.h"

namespace ikan {
  
  /// Key base event class
  class KeyEvent : public Event {
  public:
    virtual ~KeyEvent() = default;
    
    /// This function returns the key code of event
    KeyCode GetKeyCode() const;
    
    EVENT_CLASS_CATEGORY(kEventCategoryKeyboard | kEventCategoryInput);
    
  protected:
    KeyEvent(KeyCode code);
    
    KeyCode key_code_;
  };
  
  /// Key Pressed Event class
  class KeyPressedEvent : public KeyEvent {
  public:
    /// Default virtual destructor
    virtual ~KeyPressedEvent() = default;
    
    /// This this Key pressed event constructor, updates the key code and
    /// repeated value
    /// - Parameters:
    ///   - key_code: Key code of event
    ///   - repeated_count: repeat count
    KeyPressedEvent(KeyCode key_code, int32_t repeated_count);
    
    /// This function returns the count of key repeated
    int32_t GetRepeatCount() const;
    
    /// This function prints the event detail
    void Print() const override;
    
    EVENT_CLASS_TYPE(kKeyPressed);
    
  private:
    int32_t repeated_count = 0;
  };
  
  /// Key Released event class
  class KeyReleasedEvent : public KeyEvent {
  public:
    /// Default virtual destructor
    virtual ~KeyReleasedEvent() = default;
    
    /// Key Release Event
    /// - Parameter keycode: Key code that released
    KeyReleasedEvent(KeyCode keycode);
    
    /// This function prints the event detail
    void Print() const override;

    EVENT_CLASS_TYPE(kKeyReleased);
  };
  
  /// Key typed Event class
  class KeyTypedEvent : public KeyEvent {
  public:
    /// Default virtual destructor
    virtual ~KeyTypedEvent() = default;
    
    /// Key Type event
    /// - Parameter keycode: Key code pressed
    KeyTypedEvent(KeyCode keycode);

    /// This function prints the event detail
    void Print() const override;

    EVENT_CLASS_TYPE(kKeyTyped);
  };
  
} // namepsace ikan

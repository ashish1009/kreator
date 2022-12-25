//
//  key_event.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "key_event.hpp"

namespace ikan {
  
  KeyEvent::KeyEvent(KeyCode code) : key_code_(code) {}
  
  KeyCode KeyEvent::GetKeyCode() const {
    return key_code_;
  }
  
  KeyPressedEvent::KeyPressedEvent(KeyCode key_code, int32_t repeat_count)
  : KeyEvent(key_code), repeated_count(repeat_count) {}
  
  int32_t KeyPressedEvent::GetRepeatCount() const {
    return repeated_count;
  }
  
  void KeyPressedEvent::Print() const {
    IK_CORE_TRACE("Key Press Event tiggered ");
    IK_CORE_TRACE("  Key code | {0}", (uint32_t)key_code_);
    IK_CORE_TRACE("  Repeated | {0}", repeated_count);
  }

  KeyReleasedEvent::KeyReleasedEvent(KeyCode key_code)
  : KeyEvent(key_code) {}

  void KeyReleasedEvent::Print() const {
    IK_CORE_TRACE("Key Release Event tiggered ");
    IK_CORE_TRACE("  Key code | {0}", (uint32_t)key_code_);
  }

  KeyTypedEvent::KeyTypedEvent(KeyCode key_code)
  : KeyEvent(key_code) {}

  void KeyTypedEvent::Print() const {
    IK_CORE_TRACE("Key Type Event tiggered ");
    IK_CORE_TRACE("  Key code | {0}", (uint32_t)key_code_);
  }

} // namepsace ikan

//
//  mouse_codes.h
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the mouse codes in readbale language Using GLFW Key codes

namespace ikan {
  
  /// Mouse button codes
  enum class MouseButton : uint8_t {
    /* From glfw3.h */
    Button0                = 0,
    Button1                = 1,
    Button2                = 2,
    Button3                = 3,
    Button4                = 4,
    Button5                = 5,
    Button6                = 6,
    Button7                = 7,
    
    ButtonLast             = Button7,
    ButtonLeft             = Button0,
    ButtonRight            = Button1,
    ButtonMiddle           = Button2
  };
  
}

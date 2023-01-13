//
//  setting.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
  /// You can define this to inject whatever data you want in b2Body
  struct BodyUserData {
    BodyUserData() {
      pointer = 0;
    }
    
    /// For legacy compatibility
    uintptr_t pointer;
  };
}

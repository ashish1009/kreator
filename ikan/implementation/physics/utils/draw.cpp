//
//  draw.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "draw.hpp"

namespace physics {
  
  Draw::Draw() {
    draw_flags = 0;
  }
  
  void Draw::SetFlags(uint32_t flags) {
    draw_flags = flags;
  }
  
  uint32_t Draw::GetFlags() const {
    return draw_flags;
  }
  
  void Draw::AppendFlags(uint32_t flags) {
    draw_flags |= flags;
  }
  
  void Draw::ClearFlags(uint32_t flags) {
    draw_flags &= ~flags;
  }

}

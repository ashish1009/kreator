//
//  viewport.hpp
//  ecs
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

namespace ecs {
  
  using namespace ikan;
  
  struct Viewport {
    DELETE_COPY_MOVE_CONSTRUCTORS(Viewport);

    // -----------------
    // Member function
    // -----------------
    Viewport(const glm::vec4& fb_color = glm::vec4(0.0f));
  };
  
}

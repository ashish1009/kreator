//
//  aabb.hpp
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

// This class includes the AABB class to store bounding box

namespace ikan {
  
  /// Wrapper to store the AABB Box for any entity
  struct AABB {
    glm::vec3 min{-0.5f}, max{0.5f};
    
    /// This is the default Constructor for creating AABB
    AABB();
    /// This Constructor create AABB with min and max bounds
    /// - Parameters:
    ///   - min: Min bound for x, y and Z
    ///   - max: Max bound for x, y and Z
    AABB(const glm::vec3& min, const glm::vec3& max);
    
    /// Tihs function returns the World AABB bounding box of current aabb
    /// - Parameter transform: new trasnform for which aabb bounding box is needed
    static AABB GetWorldAABBPos(const glm::mat4& transform);
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(AABB)
  };
  
}

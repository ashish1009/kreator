//
//  aabb.hpp
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

namespace ikan {
  
  /// This class is the wrapper to store the AABB (Axis Alligned Bounding Box) for any entity.
  ///  - Defatult constructor generates the bouding box of the range [-0.5f : 0.5f]
  ///  - Constructor with min and max bound generates the bounding box of range [min : max]
  struct AABB {
    glm::vec3 min{-0.5f}, max{0.5f};
    
    /// This is the default Constructor for creating AABB
    AABB();
    /// This Constructor create AABB with min and max bounds
    ///  - Parameters:
    ///   - min: Min bound of aabb vector of 3 floats
    ///   - max: Max bound of aabb vector of 3 floats
    AABB(const glm::vec3& min, const glm::vec3& max);
    
    /// Tihs function generates the AABB bounding box using transform matrix of the entity
    ///  - Parameters:
    ///   - transform: Tranform matrix for updating the base AABB bouding box
    ///  - Important: Return value should not be discarded
    [[nodiscard]] static AABB GetWorldAABBPos(const glm::mat4& transform);
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(AABB)
  };
  
}

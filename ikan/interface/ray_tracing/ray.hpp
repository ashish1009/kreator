//
//  Ray.hpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

namespace ikan {
    
  /// Basic Ray class to store the data for each ray
  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    
    /// Return the point on ray at disrtace
    /// - Parameter distance: distance of point
    glm::vec3 At(float distance) const;
    
    /// Construct the ray data
    /// - Parameters:
    ///   - origin: origin of ray
    ///   - direction: direction of ray
    Ray(const glm::vec3& origin, const glm::vec3& direction);
    Ray() = default;
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(Ray)
  };
  
}

//
//  sphere.hpp
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#pragma once

namespace ikan {
  
  struct BoundingCircle {
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 radius{1.0f};
    
    /// This is the default Constructor for creating AABB
    BoundingCircle();
    /// This Constructor create Sphere with position and radius
    /// - Parameter pos: Position of point
    BoundingCircle(const glm::vec3& pos, const glm::vec3& radius = glm::vec3(1.0f), const glm::vec3& rotation = glm::vec3(0.0f));
    
    /// Tihs function returns the World AABB bounding box of current aabb
    /// - Parameters:
    ///   - position: position
    ///   - scale: scale
    ///   - rotation: rotation
    static BoundingCircle GetWorldSpherePos(const glm::vec3& position,
                                            const glm::vec3& rotation,
                                            const glm::vec3& scale);
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(BoundingCircle)

  };
  
}

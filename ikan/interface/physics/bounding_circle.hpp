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
    float radius = 1.0f;
    
    /// This is the default Constructor for creating AABB
    BoundingCircle();
    /// This Constructor create Sphere with position and radius
    /// - Parameter pos: Position of point
    BoundingCircle(const glm::vec3& pos, float radius);
    
    /// Tihs function returns the World AABB bounding box of current aabb
    /// - Parameter transform: new trasnform for which aabb bounding box is needed
    BoundingCircle GetWorldSpherePos(const glm::mat4& transform) const;
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(BoundingCircle)

  };
  
}

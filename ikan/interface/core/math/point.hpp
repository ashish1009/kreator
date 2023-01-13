//
//  point.h
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#pragma once

// This class includes the AABB class to store bounding box

namespace ikan {
  
  /// Wrapper to store the AABB Box for any entity
  struct Point {
    glm::vec3 position{0.0f};
    
    /// This is the default Constructor for creating AABB
    Point();
    /// This Constructor create Point with position
    /// - Parameter pos: Position of point
    Point(const glm::vec3& pos);
    
    /// Tihs function returns the World AABB bounding box of current aabb
    /// - Parameter transform: new trasnform for which aabb bounding box is needed
    Point GetWorldPointPos(const glm::mat4& transform) const;
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(Point)
  };
  
}

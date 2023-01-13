//
//  point.cpp
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#include "point.hpp"

namespace ikan {
  
  Point::Point() {}
  Point::Point(const glm::vec3& pos) : position(pos) { }
  Point::Point(const Point& other) : position(other.position) { }
  Point::Point(Point&& other) : position(other.position) { }
  Point& Point::operator=(const Point& other) {
    position = other.position;
    return *this;
  }
  Point& Point::operator=(Point&& other) {
    position = other.position;
    return *this;
  }
  
  Point Point::GetWorldPointPos(const glm::mat4& transform) const {
    Point point;
    // Update the Bounding box based on the transform of the mesh
    point.position = glm::vec3(transform * glm::vec4(point.position.x, point.position.y, point.position.z, 1.0));
    return point;
  }
  
} // namesapce ikan

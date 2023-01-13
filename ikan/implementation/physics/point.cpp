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
    return point;
  }
  
} // namesapce ikan

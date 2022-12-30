//
//  shape.hpp
//  ray_tracing
//
//  Created by Ashish . on 30/12/22.
//

#pragma once

#include "math.hpp"

/// This is the base class for any shape to render
class Shape {
public:
  /// This function check is ray intersect the shape
  /// - Parameters:
  ///   - ray:
  ///   - intersection:
  virtual bool Intersect(const Ray_& r,
                         SurfaceInteraction& interaction) const = 0;
};

//
//  maths.hpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

#include <glm/glm.hpp>

// This file includes the Math wrapper class using glm

namespace ikan::Math {
  
  /// This funciton decomposes the postion Transform matrix as position rotation and scale
  ///  - Parameters:
  ///   - transform: Transfrom matrix
  ///   - outTranslation: Output as position
  ///   - outRotation: output as rotation
  ///   - outScale: output as scale
  bool DecomposeTransform(const glm::mat4& transform,
                          glm::vec3& outTranslation,
                          glm::vec3& outRotation,
                          glm::vec3& outScale);
  
  /// This function generates tht transform matrix from position scale and rotation
  ///  - Parameters:
  ///   - position: position
  ///   - rotaiton: rotaiton
  ///   - scale: scale
  [[nodiscard]] glm::mat4 GetTransformMatrix(const glm::vec3& position,
                                             const glm::vec3& rotation,
                                             const glm::vec3& scale);
  
}

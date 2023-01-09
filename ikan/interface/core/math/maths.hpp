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
  
  // ------------------------------------------------
  // APIs for Transform matrix
  // ------------------------------------------------
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
                                             const glm::vec3& rotation = glm::vec3(0.0f),
                                             const glm::vec3& scale = glm::vec3(1.0f));
  
  // ------------------------------------------------
  // APIs for Random Generator
  // ------------------------------------------------
  /// This function returns the random double number
  [[nodiscard]] double RandomDouble();
  /// This function returns the random double number in range
  /// - Parameters:
  ///   - min: Min range
  ///   - max: max range
  [[nodiscard]] double RandomDouble(double min, double max);
  /// This function returns the random vec 3
  [[nodiscard]] glm::vec3 RandomVec3();
  /// This function returns the random vec 3 in range
  /// - Parameters:
  ///   - min: range min
  ///   - max: range max
  [[nodiscard]] glm::vec3 RandomVec3(double min, double max);
  /// This funtion return the random vec3 in unit sphere
  [[nodiscard]] glm::vec3 RandomInUnitSphere();
  
  // ------------------------------------------------
  // APIs for Vec 3 data
  // ------------------------------------------------
  /// This function returns the length of vec 3
  /// - Parameter v: vec3
  float Vec3Length(const glm::vec3& v);
  /// This function returns the squared length of vec 3
  /// - Parameter v: vec 3
  float Vec3LengthSquared(const glm::vec3& v);
  /// This function returns the unit vector
  /// - Parameter v: vector
  glm::vec3 UnitVector(const glm::vec3& v);

  // ------------------------------------------------
  // APIs
  // ------------------------------------------------
  /// This function refrect the light
  /// - Parameters:
  ///   - uv: light direction
  ///   - n: normal
  ///   - etai_over_etat: ratiio of refractive index
  glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat);
  
}

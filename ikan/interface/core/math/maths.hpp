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
  /// This funciton decomposes the Transform matrix as position rotation and scale
  ///  - Parameters:
  ///   - transform: Transfrom matrix to be decomposed
  ///   - out_translation: Position output
  ///   - out_rotation: Rotation output
  ///   - out_scale: Scale output
  void DecomposeTransform(const glm::mat4& transform,
                          glm::vec3& out_translation,
                          glm::vec3& out_rotation,
                          glm::vec3& out_scale);
  
  
  /// This function generates and returns the transform matrix from position scale and rotation
  ///  - Parameters:
  ///   - position: Position of the component
  ///   - rotaiton: Rotaiton of the component : Default glm::vec3(0.0f)
  ///   - scale: Scale of the component : Default glm::vec3(1.0f)
  ///  - Important: Return value should not be discarded
  [[nodiscard]] glm::mat4 GetTransformMatrix(const glm::vec3& position,
                                             const glm::vec3& rotation = glm::vec3(0.0f),
                                             const glm::vec3& scale = glm::vec3(1.0f));
  
  // ------------------------------------------------
  // APIs for Random Generator
  // ------------------------------------------------
  /// This function returns the random number 'double'
  ///  - Important: Return value should not be discarded
  [[nodiscard("Ignoring high cycle consuming random number")]] double RandomDouble();
  /// This function returns the random number 'double' in a range
  ///  - Parameters:
  ///   - min: Min range of random number
  ///   - max: max range of random number
  ///  - Important: Return value should not be discarded
  [[nodiscard("Ignoring high cycle consuming random number")]] double RandomDouble(double min, double max);
  /// This function returns the random vector of 3 floats
  ///  - Important: Return value should not be discarded
  [[nodiscard("Ignoring high cycle consuming random number")]] glm::vec3 RandomVec3();
  /// This function returns the random vector of 3 floats in a range
  ///  - Parameters:
  ///   - min: Min range of random number
  ///   - max: Max range of random number
  ///  - Important: Return value should not be discarded
  [[nodiscard("Ignoring high cycle consuming random number")]] glm::vec3 RandomVec3(double min, double max);
  /// This function returns the random vector of 3 floats with value in a unit sphere [-1, 1]
  ///  - Important: Return value should not be discarded
  [[nodiscard("Ignoring high cycle consuming random number")]] glm::vec3 RandomInUnitSphere();
  
  // ------------------------------------------------
  // APIs for Vec 3 data
  // ------------------------------------------------
  /// This function returns the length of Vector of 3 floats
  ///  - Parameter v: value of glm::vec3 for which length need to be calculated
  float Vec3Length(const glm::vec3& v);
  /// This function returns the squared length  of Vector of 3 floats
  ///  - Parameter v: vec 3
  float Vec3LengthSquared(const glm::vec3& v);
  /// This function returns the unit  Vector of 3 floats
  ///  - Parameter v: vector
  ///  - Important: Return value should not be discarded
  ///  - Returns: The Unit Vector of 3 floats
  glm::vec3 UnitVector(const glm::vec3& v);

  // ------------------------------------------------
  // APIs
  // ------------------------------------------------
  /// This function returns the direction of refrected ray from the normal vector
  ///  - Parameters:
  ///   - uv: Incoming Light direction
  ///   - n: Normal of the plane (contact point)
  ///   - etai_over_etat: Ratio of refractive index
  ///  - Important: Return value should not be discarded
  [[nodiscard]] glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat);
    
}

//
//  maths.cpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#include "maths.hpp"

namespace ikan::Math {
  
  bool DecomposeTransform(const glm::mat4& transform,
                          glm::vec3& translation,
                          glm::vec3& rotation,
                          glm::vec3& scale) {
    // From glm::decompose in matrix_decompose.inl
    
    using namespace glm;
    using T = float;
    
    mat4 LocalMatrix(transform);
    
    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
      return false;
    
    // First, isolate perspective.  This is the messiest.
    if ( epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) or
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) or
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
      // Clear the perspective partition
      LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
      LocalMatrix[3][3] = static_cast<T>(1);
    }
    
    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);
    
    vec3 Row[3];
    
    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
      for (length_t j = 0; j < 3; ++j)
        Row[i][j] = LocalMatrix[i][j];
    
    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));
    
    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0) {
      rotation.x = atan2(Row[1][2], Row[2][2]);
      rotation.z = atan2(Row[0][1], Row[0][0]);
    } else {
      rotation.x = atan2(-Row[2][0], Row[1][1]);
      rotation.z = 0;
    }
    
    return true;
  }
  
  glm::mat4 GetTransformMatrix(const glm::vec3& position,
                               const glm::vec3& rotaiton,
                               const glm::vec3& scale) {
    glm::mat4 rotation = glm::toMat4(glm::quat(rotaiton));
    return glm::translate(glm::mat4(1.0f), position) * rotation * glm::scale(glm::mat4(1.0f), scale);
  }

  double RandomDouble() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
  }
  
  double RandomDouble(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min) * RandomDouble();
  }
  
  glm::vec3 RandomVec3() {
    return glm::vec3(RandomDouble(), RandomDouble(), RandomDouble());
  }
  
  glm::vec3 RandomVec3(double min, double max) {
    return glm::vec3(RandomDouble(min,max), RandomDouble(min,max), RandomDouble(min,max));
  }

  glm::vec3 RandomInUnitSphere() {
    while (true) {
      auto p = RandomVec3(-1,1);
      if (glm::normalize(p).x >= 1) continue;
      return p;
    }
  }
  
  float Vec3Length(const glm::vec3& v) {
    return sqrt(Math::Vec3LengthSquared(v));
  }
  
  float Vec3LengthSquared(const glm::vec3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
  }
  
  glm::vec3 UnitVector(glm::vec3 v) {
    return v / Vec3Length(v);
  }

  
  glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat) {
    float cos_theta = fmin(dot(-uv, n), 1.0);
    glm::vec3 r_out_perp =  etai_over_etat * (uv + cos_theta * n);
    glm::vec3 r_out_parallel = -sqrt(fabs((float)1.0 - Math::Vec3LengthSquared(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
  }

} 

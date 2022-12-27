//
//  camera.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "camera.hpp"

namespace ikan {
  
  Camera::Camera(float aspect_ratio,
                 float near_plane,
                 float far_plane)
  : aspect_ratio_(aspect_ratio), near_plane_(near_plane), far_plane_(far_plane) {
  }
  
  Camera& Camera::operator=(const Camera& other) {
    near_plane_ = other.near_plane_;
    far_plane_ = other.far_plane_;
    aspect_ratio_ = other.aspect_ratio_;
    projection_matrix_ = other.projection_matrix_;
    
    return *this;
  }
  
  void Camera::SetNear(float near) { near_plane_ = near; }
  void Camera::SetFar(float far) { far_plane_ = far;}
  
  float Camera::GetNear() const { return near_plane_; }
  float Camera::GetFar() const { return far_plane_; }
  const glm::mat4& Camera::GetProjectionMatrix() const { return projection_matrix_; }
  
  float Camera::GetAspectRatio() const { return aspect_ratio_; }
  
}

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
  
  void Camera::SetNear(float near) { near_plane_ = near; }
  void Camera::SetFar(float far) { far_plane_ = far;}
  
  float Camera::GetNear() const { return near_plane_; }
  float Camera::GetFar() const { return far_plane_; }
  const glm::mat4& Camera::GetProjection() const { return projection_matrix_; }
  
  float Camera::GetAspectRatio() const { return aspect_ratio_; }
  
}

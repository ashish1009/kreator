//
//  editor_camera.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "editor_camera.hpp"

namespace ikan {
  
  EditorCamera::EditorCamera(float aspect_ratio,
                             float fov,
                             float near_plane,
                             float far_plane)
  : Camera(aspect_ratio, near_plane, far_plane), fov_(fov) {
    projection_matrix_ = glm::perspective(fov_,
                                          aspect_ratio_,
                                          near_plane_,
                                          far_plane_);

    position_ = { -15, 5, 5};
    distance_ = glm::distance(position_, focal_point_);
    
    // Update the camera view matrix
    UpdateCameraView();
    
    IK_CORE_INFO("Creating Editor Camera ... ");
    IK_CORE_INFO("  FOV          | {0} degree", glm::degrees(fov_));
    IK_CORE_INFO("  Aspect Ratio | {0}", aspect_ratio_);
    IK_CORE_INFO("  Near Plane   | {0}", near_plane_);
    IK_CORE_INFO("  Far Plane    | {0}", far_plane_);
    IK_CORE_INFO("  FocalPoint   | {0} | {1} | {2}", focal_point_.x, focal_point_.y, focal_point_.z);
  }
  
  EditorCamera::~EditorCamera() {
    IK_CORE_WARN("Destroying Editor Camera !!!");
    IK_CORE_WARN("  FOV          | {0} degree", glm::degrees(fov_));
    IK_CORE_WARN("  Aspect Ratio | {0}", aspect_ratio_);
    IK_CORE_WARN("  Near Plane   | {0}", near_plane_);
    IK_CORE_WARN("  Far Plane    | {0}", far_plane_);
    IK_CORE_WARN("  FocalPoint   | {0} | {1} | {2}", focal_point_.x, focal_point_.y, focal_point_.z);
  }
  
  void EditorCamera::UpdateCameraView() {
    position_ = CalculatePosition();
    glm::quat orientation = GetOrientation();
    
    rotation_   = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
    view_matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::toMat4(orientation);
    view_matrix_ = glm::inverse(view_matrix_);
  }
  
  glm::vec3 EditorCamera::CalculatePosition() const {
    return focal_point_ - GetForwardDirection() * distance_;
  }
  glm::quat EditorCamera::GetOrientation() const {
    return glm::quat(glm::vec3(-pitch_, -yaw_, 0.0f));
  }
  
  glm::mat4 EditorCamera::GetViewProjection() const {
    return projection_matrix_ * view_matrix_;
  }
  const glm::mat4& EditorCamera::GetView() const {
    return view_matrix_;
  }
  
  const glm::vec3& EditorCamera::GetPosition() const {
    return position_;
  }
  const glm::vec3& EditorCamera::GetFocalPoint() const {
    return focal_point_;
  }

  glm::vec3 EditorCamera::GetUpDirection() const {
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
  }
  glm::vec3 EditorCamera::GetRightDirection() const {
    return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
  }
  glm::vec3 EditorCamera::GetForwardDirection() const {
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
  }

}

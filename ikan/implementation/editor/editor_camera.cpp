//
//  editor_camera.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "editor_camera.hpp"
#include "core/input.hpp"

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
  
  void EditorCamera::Update([[maybe_unused]] Timestep ts) {
    if (Input::IsKeyPressed(KeyCode::LeftAlt)) {
      const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
      glm::vec2 delta = (mouse - initial_mouse_position_) * 0.003f;
      initial_mouse_position_ = mouse;
      
      if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft))
        MouseRotate(delta);
    }
    if (Input::IsKeyPressed(KeyCode::LeftControl)) {
      const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
      glm::vec2 delta = (mouse - initial_mouse_position_) * 0.003f;
      initial_mouse_position_ = mouse;
      
      if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft))
        MousePan(delta);
    }
    if (Input::IsKeyPressed(KeyCode::LeftSuper)) {
      const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
      glm::vec2 delta = (mouse - initial_mouse_position_) * 0.003f;
      initial_mouse_position_ = mouse;
      
      if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft))
        MouseZoom(delta.y);
    }
    UpdateCameraView();
  }
  
  void EditorCamera::SetViewportSize(uint32_t width, uint32_t height) {
    if (height == 0)
      return;
    
    viewport_width_  = width;
    viewport_height_ = height;
    aspect_ratio_    = (float)viewport_width_ / (float)viewport_height_;
    
    projection_matrix_ = glm::perspective(fov_, aspect_ratio_, near_plane_, far_plane_);
    UpdateCameraView();
    IK_CORE_TRACE("Changing Viewport Size of Editor Camera : {0} x {1}."
                  "(NOTE: Updating View Projection Matrix)", width, height);
  }

  void EditorCamera::UpdateCameraView() {
    position_ = CalculatePosition();
    glm::quat orientation = GetOrientation();
    
    rotation_   = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
    view_matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::toMat4(orientation);
    view_matrix_ = glm::inverse(view_matrix_);
  }
  
  void EditorCamera::MouseZoom(float delta) {
    distance_ -= delta * ZoomSpeed();
    if (distance_ < 0.0f) {
      // Not changing focal point wit Zoom
      // focal_point_ += GetForwardDirection();
      distance_ = 0.0f;
    }
  }
  
  void EditorCamera::MousePan(const glm::vec2& delta) {
    auto speed = PanSpeed();
    focal_point_ += -GetRightDirection() * delta.x * speed.x * distance_;
    focal_point_ += GetUpDirection() * delta.y * speed.y * distance_;
  }
  
  void EditorCamera::MouseRotate(const glm::vec2& delta) {
    float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
    yaw_   += yawSign * delta.x * RotationSpeed();
    pitch_ += delta.y * RotationSpeed();
  }
  
  glm::vec2 EditorCamera::PanSpeed() const {
    float x = std::min(viewport_width_ / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
    
    float y = std::min(viewport_height_ / 1000.0f, 2.4f); // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
    
    return { xFactor, yFactor };
  }
  
  float EditorCamera::RotationSpeed() const {
    return 0.8f;
  }
  
  float EditorCamera::ZoomSpeed() const {
    float distance = distance_ * 0.2f;
    distance = std::max(distance, 0.0f);
    
    float speed = distance * distance;
    speed = std::min(speed, 100.0f);
    
    return speed;
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

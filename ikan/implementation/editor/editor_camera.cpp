//
//  editor_camera.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "editor_camera.hpp"
#include "core/input.hpp"
#include "editor/property_grid.hpp"

namespace ikan {
  
  EditorCamera::EditorCamera(bool update_rays,
                             float aspect_ratio,
                             float fov,
                             float near_plane,
                             float far_plane)
  : Camera(aspect_ratio, near_plane, far_plane), fov_(fov), update_rays_(update_rays) {
    UpdateCameraProjection();
    
    position_ = { -15, 5, 5};
    distance_ = glm::distance(position_, focal_point_);
    
    // Update the camera view matrix
    UpdateCameraView();
    
    // Update the ray directions
    UpdateRayDirections();
    
    IK_CORE_TRACE(LogModule::EditorCamera, "Creating Editor Camera ... ");
    IK_CORE_TRACE(LogModule::EditorCamera, "  FOV          | {0} degree", glm::degrees(fov_));
    IK_CORE_TRACE(LogModule::EditorCamera, "  Aspect Ratio | {0}", aspect_ratio_);
    IK_CORE_TRACE(LogModule::EditorCamera, "  Near Plane   | {0}", near_plane_);
    IK_CORE_TRACE(LogModule::EditorCamera, "  Far Plane    | {0}", far_plane_);
    IK_CORE_TRACE(LogModule::EditorCamera, "  FocalPoint   | {0} | {1} | {2}", focal_point_.x, focal_point_.y, focal_point_.z);
  }
  
  EditorCamera::~EditorCamera() {
    IK_CORE_WARN(LogModule::EditorCamera, "Destroying Editor Camera !!!");
    IK_CORE_WARN(LogModule::EditorCamera, "  FOV          | {0} degree", glm::degrees(fov_));
    IK_CORE_WARN(LogModule::EditorCamera, "  Aspect Ratio | {0}", aspect_ratio_);
    IK_CORE_WARN(LogModule::EditorCamera, "  Near Plane   | {0}", near_plane_);
    IK_CORE_WARN(LogModule::EditorCamera, "  Far Plane    | {0}", far_plane_);
    IK_CORE_WARN(LogModule::EditorCamera, "  FocalPoint   | {0} | {1} | {2}", focal_point_.x, focal_point_.y, focal_point_.z);
  }
  
  bool EditorCamera::Update([[maybe_unused]] Timestep ts) {
    bool moved = false;
    if (new_update_) {
      glm::vec2 mouse_position = Input::GetMousePosition();
      glm::vec2 delta = (mouse_position - initial_mouse_position_) * 0.002f;
      initial_mouse_position_ = mouse_position;
      
      if (!Input::IsMouseButtonPressed(MouseButton::ButtonRight)) {
        Input::SetCursorMode(CursorMode::Normal);
        return false;
      }
      Input::SetCursorMode(CursorMode::Locked);
      
      // Movement
      if (Input::IsKeyPressed(KeyCode::W)) {
        focal_point_ -= GetUpDirection() * MoveSpeed() * (float)ts;
        moved = true;
      }
      else if (Input::IsKeyPressed(KeyCode::S)){
        focal_point_ += GetUpDirection() * MoveSpeed() * (float)ts;
        moved = true;
      }
      else if (Input::IsKeyPressed(KeyCode::D)) {
        focal_point_ -= GetRightDirection() * MoveSpeed() * (float)ts;
        moved = true;
      }
      if (Input::IsKeyPressed(KeyCode::A)) {
        focal_point_ += GetRightDirection() * MoveSpeed() * (float)ts;
        moved = true;
      }
      if (Input::IsKeyPressed(KeyCode::Q)) {
        focal_point_ -= GetForwardDirection() * MoveSpeed() * (float)ts;
        moved = true;
      }
      else if (Input::IsKeyPressed(KeyCode::E)) {
        focal_point_ += GetForwardDirection() * MoveSpeed() * (float)ts;
        moved = true;
      }
      
      // Rotation
      if (delta.x != 0.0f or delta.y != 0.0f) {
        MouseRotate(delta);
        moved = true;
      }
    } else {
      if (Input::IsKeyPressed(KeyCode::LeftAlt)) {
        const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
        glm::vec2 delta = (mouse - initial_mouse_position_) * 0.003f;
        initial_mouse_position_ = mouse;
        
        if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft)) {
          moved = true;
          MouseRotate(delta);
        }
      }
      if (Input::IsKeyPressed(KeyCode::LeftControl)) {
        const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
        glm::vec2 delta = (mouse - initial_mouse_position_) * 0.003f;
        initial_mouse_position_ = mouse;
        
        if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft)) {
          moved = true;
          MousePan(delta);
        }
      }
      if (Input::IsKeyPressed(KeyCode::LeftSuper)) {
        const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
        glm::vec2 delta = (mouse - initial_mouse_position_) * 0.003f;
        initial_mouse_position_ = mouse;
        
        if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft)) {
          moved = true;
          MouseZoom(delta.y);
        }
      }
    }
    
    if (moved) {
      UpdateCameraView();
      UpdateRayDirections();
    }

    return moved;
  }
  
  bool EditorCamera::EventHandler(Event& e) {
    EventDispatcher dispatcher(e);
    return dispatcher.Dispatch<MouseScrolledEvent>(IK_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
  }
  
  bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
    float delta = e.GetYOffset() * 0.1f;
    MouseZoom(delta);
    
    UpdateCameraView();
    UpdateRayDirections();
    return false;
  }
  
  void EditorCamera::SetViewportSize(uint32_t width, uint32_t height) {
    if (height == 0 or (viewport_width_ == width and viewport_height_ == height))
      return;
    
    viewport_width_  = width;
    viewport_height_ = height;
    aspect_ratio_    = (float)viewport_width_ / (float)viewport_height_;
    
    UpdateCameraProjection();
    UpdateCameraView();
    UpdateRayDirections();
    IK_CORE_TRACE(LogModule::EditorCamera, "Changing Viewport Size of Editor Camera : {0} x {1}."
                  "(NOTE: Updating View Projection Matrix)", width, height);
  }
  
  void EditorCamera::UpdateCameraProjection() {
    projection_matrix_ = glm::perspective(fov_,
                                          aspect_ratio_,
                                          near_plane_,
                                          far_plane_);
    inverse_projection_ = glm::inverse(projection_matrix_);
  }

  void EditorCamera::UpdateCameraView() {
    position_ = CalculatePosition();
    glm::quat orientation = GetOrientation();
    
    rotation_   = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
    view_matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::toMat4(orientation);
    view_matrix_ = glm::inverse(view_matrix_);
    
    projection_view_matrix_ = projection_matrix_ * view_matrix_;
    inverse_view_ = glm::inverse(view_matrix_);
  }
  
  void EditorCamera::UpdateRayDirections() {
    if (!update_rays_)
      return;
    
    ray_directions_.resize(viewport_width_ * viewport_height_);
    
    dispatch_apply(viewport_height_, loop_dispactch_queue_, ^(size_t y) {
      dispatch_apply(viewport_width_, loop_dispactch_queue_, ^(size_t x) {
        glm::vec2 coord = { (float)x / (float)viewport_width_, (float)y / (float)viewport_height_ };
        coord = coord * 2.0f - 1.0f; // -1 -> 1
        
        glm::vec4 target = inverse_projection_ * glm::vec4(coord.x, coord.y, 1, 1);
        glm::vec3 ray_direction = glm::vec3(inverse_view_ * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
        ray_directions_[x + y * viewport_width_] = ray_direction;
      });
    });
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
  
  float EditorCamera::MoveSpeed() const {
    return 10.0f;
  }
  
  float EditorCamera::ZoomSpeed() const {
    float distance = distance_ * 0.2f;
    distance = std::max(distance, 0.0f);
    
    float speed = distance * distance;
    speed = std::min(speed, 100.0f);
    
    return speed;
  }
  
  void EditorCamera::SetPosition(const glm::vec3& new_position) {
    position_ = new_position;
    distance_ = glm::distance(position_, focal_point_);
    
    // Update the camera view matrix
    UpdateCameraView();
    
    // Update the ray directions
    UpdateRayDirections();
  }
  
  void EditorCamera::RendererGui() {
    ImGui::Begin("Editor Camera");
    ImGui::PushID("Editor Camera");
    
    static float fov_angle = glm::degrees(fov_);
    bool modified = false;
    if (PropertyGrid::Float1("FOV", fov_angle, nullptr, 1.0f, 10.0f, 120.0f)) {
      fov_ = glm::radians(fov_angle);
      modified = true;
    }
    ImGui::Separator();
    
    if (PropertyGrid::Float3("Focal Point", focal_point_, nullptr, 0.1f)) {
      modified = true;
    }
    if (PropertyGrid::Float1("Distance", distance_, nullptr, 0.1f)) {
      modified = true;
    }

    if (modified) {
      UpdateCameraProjection();
      UpdateCameraView();
      UpdateRayDirections();
    }
    
    ImGui::PopID();
    ImGui::End();
  }
  
  glm::vec3 EditorCamera::CalculatePosition() const {
    return focal_point_ - GetForwardDirection() * distance_;
  }
  glm::quat EditorCamera::GetOrientation() const {
    return glm::quat(glm::vec3(-pitch_, -yaw_, 0.0f));
  }
  
  const glm::mat4& EditorCamera::GetViewProjection() const {
    return projection_view_matrix_;
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
  
  uint32_t EditorCamera::GetViewportHeight() const {
    return viewport_height_;
  }
  uint32_t EditorCamera::GetViewportWidth() const {
    return viewport_width_;
  }
  const std::vector<glm::vec3>& EditorCamera::GetRayDirections() const {
    return ray_directions_;
  }

}

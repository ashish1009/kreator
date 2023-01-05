//
//  editor_camera.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#pragma once

// This class includes the Editor camera class

#include "renderer/utils/camera.hpp"
#include "core/utils/time_step.hpp"
#include "core/events/mouse_event.hpp"

namespace ikan {
    
  class EditorCamera : public Camera {
  public:
    // ----------------------------
    // Constructors and Destructor
    // ----------------------------
    /// This constructor creates the Editor Camera instance and initialize all the parameters
    /// - Parameters:
    ///   - update_rays: update rays for each frame
    ///   - aspect_ratio: Aspect ratio of the famera
    ///   - fov: FOV of the camera in radians
    ///   - near_plane: Near plane of camera
    ///   - far_plane: far plane of camera
    EditorCamera(bool update_rays = false,
                 float aspect_ratio = 16.0f / 9.0f,
                 float fov = glm::radians(75.0f),
                 float near_plane = 0.01f,
                 float far_plane = 100000.0f);
    /// This destructor destroys the Editor Camera instnace
    ~EditorCamera();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(EditorCamera);
    
    /// This function updates the Parameter of Editor Camera each frame
    /// - Parameter ts: time step of each frame
    bool Update([[maybe_unused]] Timestep ts);
    /// This function handles Events of Editor Camera
    /// - Parameter e: event instance
    bool EventHandler(Event& e);
    /// This function renders Gui Data for Editor Camera
    /// - Parameter is_open: flag to render or hide the widget
    void RendererGui(bool *is_open = nullptr);

    /// This function updates the viewport size for Editor Camera
    /// - Parameters:
    ///   - width: New width
    ///   - height: new height
    void SetViewportSize(uint32_t width, uint32_t height);

    /// This function handles Mouse Scroll event
    /// - Parameter e: Mouse Scroll Event
    bool OnMouseScroll(MouseScrolledEvent& e);
    /// This function update the camera position
    /// - Parameter new_position: new position
    void SetPosition(const glm::vec3& new_position);

    // ---------------
    // Getters
    // ---------------
    /// This function returns View Projection Matrix
    [[nodiscard]] const glm::mat4& GetViewProjection() const;
    /// This function returns the View Matrix
    [[nodiscard]] const glm::mat4& GetView() const;
    
    /// This function returns the Position of Editor camera
    [[nodiscard]] const glm::vec3& GetPosition() const;
    /// This function returns the Focal point of Editor camera
    [[nodiscard]] const glm::vec3& GetFocalPoint() const;
    /// This function returns the editor camera viewport Height
    [[nodiscard]] uint32_t GetViewportHeight() const;
    /// This function returns the editor camera viewport width
    [[nodiscard]] uint32_t GetViewportWidth() const;
    /// This function returns the ray Directions of the camera
    [[nodiscard]] const std::vector<glm::vec3>& GetRayDirections() const;

  private:
    // -------------------
    // Member Functions
    // -------------------
    /// This function updates the camera view matrix
    void UpdateCameraView();
    /// This function updates the camera Projection matrix
    void UpdateCameraProjection();
    /// This function calculates the ray directions
    void UpdateRayDirections();
    /// This function updates the zoom value of the camera
    void MouseZoom(float delta);
    /// This function updates the mouse pan value
    void MousePan(const glm::vec2& delta);
    /// This function updates the Mouse rotate value
    void MouseRotate(const glm::vec2& delta);

    /// This function returns the pan speed of mouse
    glm::vec2 PanSpeed() const;
    /// This function returns the rotation speed of mouse
    float RotationSpeed() const;
    /// This function returns the Movement speed of mouse
    float MoveSpeed() const;
    /// This function returns the rotation speed of camera
    float ZoomSpeed() const;

    /// This function Calculate and return the position of camera
    glm::vec3 CalculatePosition() const;
    /// This function Calculate and return the orientation of camera
    glm::quat GetOrientation() const;

    /// This function returns Up Direction of Camera
    glm::vec3 GetUpDirection() const;
    /// This function returns Right Direction of Camera
    glm::vec3 GetRightDirection() const;
    /// This function returns Forward Direction of Camera
    glm::vec3 GetForwardDirection() const;

    // -------------------
    // Member variables
    // -------------------
    glm::mat4 view_matrix_ = glm::mat4(1.0f);
    glm::mat4 projection_view_matrix_ = glm::mat4(1.0f);
    glm::mat4 inverse_projection_{ 1.0f };
    glm::mat4 inverse_view_{ 1.0f };
    
    std::vector<glm::vec3> ray_directions_;

    glm::vec3 forward_direction_{0.0f, 0.0f, 0.0f};
    glm::vec3 position_ = glm::vec3(0.0f);
    glm::vec3 rotation_ = glm::vec3(0.0f);
    glm::vec3 focal_point_ = glm::vec3(0.0f);
    
    glm::vec2 initial_mouse_position_ = glm::vec2(0.0f);
    uint32_t viewport_width_ = 1280, viewport_height_ = 720;

    float fov_ = glm::radians(75.0f);
    float distance_ = 0.0f;
    float pitch_ = 0.4f, yaw_ = 0.5f;
    
    bool new_update_ = true;
    bool update_rays_ = false;
  };
  
}

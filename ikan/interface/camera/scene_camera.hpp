//
//  scene_camera.hpp
//  ikan
//
//  Created by Ashish . on 06/01/23.
//

#pragma once

#include "camera/camera.hpp"

// This file includes the Scene camera class

namespace ikan {
  
  /// Scene Camera class to store the projection for active scene
  class SceneCamera : public Camera {
  public:
    /// Projection type of Scene Camera
    /// - Perspective for 3D
    /// - Orthographics for 2D
    enum class ProjectionType {
      Perspective = 0, Orthographic = 1
    };
    
    /// This constructor creates the Scene Camera instance
    /// - Parameter proj_type: Projection type
    SceneCamera(ProjectionType proj_type = ProjectionType::Orthographic);
    /// Default Scene Camera Desctructorr
    virtual ~SceneCamera();
    
    /// This function updates the Near plane of camera
    /// - Parameter near: New near plane
    void SetNear(float near) override;
    /// This function updates the Far plane of camera
    /// - Parameter far: new fat plane
    void SetFar(float far) override;
    /// This function returns the zoom value of camera
    float GetZoom() const override;

    /// This function renders Imgui pannel for Scene should cal this function between Imgui::Begin and Imgui::End
    void RenderGui();
    /// This function renders Imgui pannel for Scene Camera
    void RenderGuiWithTitle();
    
    /// This function renders the grid
    /// - Parameters:
    ///   - max_lines: Max line to render in grids
    ///   - line_color: Color of Grids
    ///   - camera_transform: Camera transform from component
    void RenderGrids(uint32_t max_lines, const glm::vec4& line_color, const glm::mat4& camera_transform);

    /// This function updates the new Orthographic Matrix (Only for Orthographic camera)
    /// - Parameters:
    ///   - size: new size of Camera
    ///   - near_clip: new near clip
    ///   - far_clip: new far clip
    void SetOrthographic(float size, float near_clip, float far_clip);
    /// This function updates the new Perspective Matrix (Only for Perspective camera)
    /// - Parameters:
    ///   - fov: new FOV
    ///   - near_clip: new near clip
    ///   - far_clip: new far clip
    void SetPerspective(float fov, float near_clip, float far_clip);
    
    /// This function updates the Viewport size of camera
    /// - Parameters:
    ///   - width: new width
    ///   - height: new height
    void SetViewportSize(uint32_t width, uint32_t height);
    
    /// This function updates the Camera projection type
    /// - Parameter type: new projection type
    void SetProjectionType(ProjectionType type);
    /// This function updates the Size of Orthographic Camera (Only for Orthographic Camera)
    /// - Parameter size: new size
    void SetOrthographicSize(float size);
    /// This function updates FOV of Perspective Camera (Only for Perspective camera)
    /// - Parameter fov: new FOV
    void SetPerspectiveFOV(float fov);
    
    /// This function returns the Projection type of camera
    ProjectionType GetProjectionType() const;
    /// This function returns size of Orthographic Camera
    float GetOrthographicSize() const;
    /// This function returns FOV of Perspective Camera
    float GetPerspectiveFOV() const;
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(SceneCamera);

  private:
    // Member functions
    /// This function recalculate the projection matrix
    void RecalculateProjection();
    
    // Member variables
    ProjectionType projection_type_ = ProjectionType::Orthographic;
    float perspective_fov_ = glm::radians(45.0f);
    float orthographic_size_ = 10.0f;
  };
  
}

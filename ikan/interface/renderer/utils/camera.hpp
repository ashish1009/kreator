//
//  camera.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#pragma once

// This file includes the camera base class

namespace ikan {
    
  /// This class is the Base class of camera. Both the cameras should derived from this base class
  class Camera {
  public:
    /// Default Camera constructor
    /// - Parameters:
    ///   - aspect_ratio: Aspect ratio of the famera
    ///   - near_plane: Near plane of camera
    ///   - far_plane: far plane of camera
    Camera(float aspect_ratio = 16.0f / 9.0f,
           float near_plane = 0.0f,
           float far_plane = 0.0f);
    /// Default Camera destructor
    virtual ~Camera() = default;
    
    /// This function updates the Near plane of camera
    /// - Parameter near: New near plane
    virtual void SetNear(float near);
    /// This function updates the Far plane of camera
    /// - Parameter far: new fat plane
    virtual void SetFar(float far);
    
    // --------------------
    // Getters
    // --------------------
    /// This function returns the Near Plane value
    [[nodiscard]] float GetNear() const;
    /// This function returns the Far Plane value
    [[nodiscard]] float GetFar() const;
    /// This function returns the Aspect Ratio value
    [[nodiscard]] float GetAspectRatio() const;
    /// This function returns the Projection Matrix of Camera
    [[nodiscard]] const glm::mat4& GetProjection() const;
        
    DELETE_COPY_MOVE_CONSTRUCTORS(Camera);
    
  protected:
    float near_plane_ = 0.0f;
    float far_plane_ = 0.0f;
    float aspect_ratio_ = 16.0f / 9.0f;
    
    glm::mat4 projection_matrix_ = glm::mat4(1.0f);
  };
  
} 

//
//  renderer.hpp
//  ikan
//
//  Created by Ashish . on 01/01/23.
//

#pragma once

#include "ray_scene.hpp"
#include "renderer/graphics/texture.hpp"
#include "camera/editor_camera.hpp"

namespace ikan {
    
  class RayRenderer {
  public:
    struct Setting {
      bool accumulate = true;
      bool render = true;
    };
    
    /// Default destructor
    RayRenderer() = default;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(RayRenderer)
    
    /// This function renders the scene using editor camera
    /// - Parameters:
    ///   - scene: scene reference
    ///   - camera: editor camera reference
    void Render(const RayScene& scene, const EditorCamera& camera);
    /// This function resize the rendere
    /// - Parameters:
    ///   - width: view port width
    ///   - height: view port height
    void Resize(uint32_t width, uint32_t height);
    /// This function reset the accumulation
    void ResetFrameIndex();

    // ----------------------
    // Getters
    // ----------------------
    /// This function returns the image pointer
    std::shared_ptr<Image> GetFinalImage() const;
    /// This function returns the setting reference
    Setting& GetSetting();
    
  private:
    // Member function
    /// This function returns the color value of each pixel
    /// - Parameters:
    ///   - x: x index of pixle
    ///   - y: y index of pixel
    glm::vec4 PerPixel(uint32_t x, uint32_t y);
    /// This function trace the rays on the hitable objects
    /// - Parameters:
    ///   - ray: ray of camera
    ///   - payload: hit ppayload output
    HitPayload TraceRay(const Ray& ray);
    /// This function trace the  Close hitrays on the hitable objects
    /// - Parameters:
    ///   - ray: ray of camera
    ///   - hit_distance: hti distance
    ///   - object_idx: object index
    HitPayload ClosestHit(const Ray& ray, float hit_distance, int32_t object_idx);
    /// This function trace the Miss rays on the hitable objects
    /// - Parameters:
    ///   - ray: ray of camera
    HitPayload Miss(const Ray& ray);

    // Member variables
    std::shared_ptr<Image> final_image_ = nullptr;
    uint32_t* image_data_ = nullptr;

    glm::vec4* accumulation_data_ = nullptr;
    uint32_t frame_index_ = 1;
    
    const RayScene* active_scene_ = nullptr;
    const EditorCamera* active_camera_ = nullptr;
    
    Setting setting_;
  };

}

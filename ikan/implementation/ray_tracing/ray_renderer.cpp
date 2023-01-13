//
//  renderer.cpp
//  ikan
//
//  Created by Ashish . on 01/01/23.
//

#include "ray_renderer.hpp"

namespace ray_tracing {

  static uint32_t ConevrtToRgba(const glm::vec4& pixel) {
    uint8_t r = uint8_t(pixel.r * 255.0f);
    uint8_t g = uint8_t(pixel.g * 255.0f);
    uint8_t b = uint8_t(pixel.b * 255.0f);
    uint8_t a = uint8_t(pixel.a * 255.0f);
    return ( (a << 24) | (b << 16) | (g << 8) | r);
  }

  void RayRenderer::Resize(uint32_t width, uint32_t height) {
    if (final_image_) {
      // No resize
      if (final_image_->GetWidth() == width and final_image_->GetHeight() == height)
        return;
      
      final_image_->Resize(width, height);
    }
    else {
      final_image_ = Image::Create(width, height, TextureFormat::RGBA);
    }
    
    delete[] image_data_ ;
    image_data_ = new uint32_t[width * height];
    
    delete[] accumulation_data_ ;
    accumulation_data_ = new glm::vec4[width * height];
  }
  
  void RayRenderer::Render(const RayScene &scene, const EditorCamera &camera) {
    active_scene_ = &scene;
    active_camera_ = &camera;
    
    if (setting_.render) {
      if (frame_index_ == 1) {
        memset(accumulation_data_, 0, final_image_->GetWidth() * final_image_->GetHeight());
      }
      
      dispatch_apply(final_image_->GetHeight(), loop_dispactch_queue_, ^(size_t y_) {
        dispatch_apply(final_image_->GetWidth(), loop_dispactch_queue_, ^(size_t x_) {
          uint32_t x = (uint32_t)x_;
          uint32_t y = (uint32_t)y_;
          
          uint32_t pixel_idx = x + y * final_image_->GetWidth();
          glm::vec4 pixel = PerPixel(x, y);
          
          pixel = glm::clamp(pixel, glm::vec4(0.0f), glm::vec4(1.0f));
          if (frame_index_ == 1)
            accumulation_data_[pixel_idx] = pixel;
          else
            accumulation_data_[pixel_idx] += pixel;
          
          glm::vec4 accumulated_color = accumulation_data_[pixel_idx];
          accumulated_color /= (float)frame_index_;
          
          accumulated_color = glm::clamp(accumulated_color, glm::vec4(0.0f), glm::vec4(1.0f));
          image_data_[pixel_idx] = ConevrtToRgba(accumulated_color);
        });
      });
      final_image_->SetData(image_data_);
      
      if (setting_.accumulate) {
        frame_index_++;
      } else {
        frame_index_ = 1;
      }
    }
  }
  
  glm::vec4 RayRenderer::PerPixel(uint32_t x, uint32_t y) {
    glm::vec3 color(0.0f);
    Ray ray;
    ray.origin = active_camera_->GetPosition();
    
    uint32_t pixel_idx = x + y * final_image_->GetWidth();
    ray.direction = active_camera_->GetRayDirections().at(pixel_idx);
    
    float multiplier = 1.0f;
    int32_t bounces = 10;
    
    // Ray Color
    for (uint32_t i = 0; i < bounces; i++) {
      HitPayload payload = TraceRay(ray);
      if (payload.hit_distance < 0) {
        glm::vec3 sky_color = glm::vec3(0.6, 0.7, 0.9);
        color += sky_color * multiplier;
        break;
      }
      
      const RaySphere& sphere = active_scene_->spheres[payload.object_idx];
      const RayMaterial& material = active_scene_->materials[sphere.material_index];
      if (i == 0)
        color = material.albedo;
      glm::vec3 sphere_color;
      Ray scattered_ray;
      
      if (material.Scatter(ray, payload, sphere_color, scattered_ray)) {
        color *= sphere_color;// * multiplier;
        ray = scattered_ray;
      } else {
        break;
      }
      
      // Decreasing the energy
      multiplier *= 0.5f;
    }
    
    return glm::vec4(color, 1.0f);
  }
  
  HitPayload RayRenderer::TraceRay(const Ray& ray) {
    int32_t closest_sphere_idx = -1;
    float hit_distance = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < active_scene_->spheres.size(); i++) {
      const RaySphere& sphere = active_scene_->spheres[i];
      if (sphere.Hit(ray, hit_distance)) {
        closest_sphere_idx = (int32_t)i;
      } else {
        continue;
      }
    }
    if (closest_sphere_idx < 0)
      return Miss(ray);
    
    return ClosestHit(ray, hit_distance, closest_sphere_idx);
  }
  
  HitPayload RayRenderer::ClosestHit(const Ray& ray, float hit_distance, int32_t object_idx) {
    HitPayload payload;
    payload.hit_distance = hit_distance;
    payload.object_idx = object_idx;
    
    const RaySphere closest_sphere = active_scene_->spheres[object_idx];
    payload.world_position = ray.At(payload.hit_distance);
    payload.world_normal = (payload.world_position - closest_sphere.position) / closest_sphere.radius;
    
    payload.SetFaceNormal(ray);
    return payload;
  }
  
  HitPayload RayRenderer::Miss(const Ray& ray) {
    HitPayload payload;
    payload.hit_distance = -1;
    return payload;
  }
  
  void RayRenderer::ResetFrameIndex() { frame_index_ = 1;}
  std::shared_ptr<Image> RayRenderer::GetFinalImage() const { return final_image_; }
  RayRenderer::Setting& RayRenderer::GetSetting() { return setting_; }

}

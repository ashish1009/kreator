//
//  renderer_layer.cpp
//  kreator
//
//  Created by Ashish . on 31/12/22.
//

#include "renderer_layer.hpp"
#include "ray.hpp"

namespace ray_tracing {

  static uint32_t ConevrtToRgba(const glm::vec4& pixel) {
    uint8_t r = uint8_t(pixel.r * 255.0f);
    uint8_t g = uint8_t(pixel.g * 255.0f);
    uint8_t b = uint8_t(pixel.b * 255.0f);
    uint8_t a = uint8_t(pixel.a * 255.0f);
    return ( (a << 24) | (b << 16) | (g << 8) | r);
  }

  RendererLayer::RendererLayer() : Layer("Ray Trace Renderer Layer") {
    IK_INFO("Creating Ray Trace Renderer Layer instance ... ");
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN("Destroying Ray Trace Renderer Layer instance !!! ");
  }
  
  void RendererLayer::Attach() {
    IK_INFO("Attaching Ray Trace Renderer Layer instance");
    
    editor_camera_.SetPosition({0, 0, 6});
//    int32_t num_spheres = 20;
//    spheres.resize(num_spheres);
//    for (int32_t i = 0; i < num_spheres; i++) {
//      spheres[i].position = {0, 0, 0};
//      spheres[i].radius = 1.0f;
//    }
    
    spheres.push_back(Sphere({0, 0.0, 0}, 1, {1, 0, 1}));
    spheres.push_back(Sphere({0, -101, 0}, 100, {0, 0, 1}));
  }
  
  void RendererLayer::Detach() {
    IK_WARN("Detaching Ray Trace Renderer Layer instance ");
  }
  
  void RendererLayer::Resize() {
    if (final_image_) {
      // No resize
      if (final_image_->GetWidth() == viewport_width_ and final_image_->GetHeight() == viewport_height_)
        return;
      
      delete[] image_data_ ;
      image_data_ = new uint32_t[viewport_width_ * viewport_height_];
      final_image_->Resize(viewport_width_, viewport_height_);
    } else {
      final_image_ = Image::Create(viewport_width_, viewport_height_, TextureFormat::RGBA);
      delete[] image_data_ ;
      image_data_ = new uint32_t[viewport_width_ * viewport_height_];
    }
  }
  
  void RendererLayer::Render() {
    dispatch_apply(final_image_->GetHeight(), loop_dispactch_queue_, ^(size_t y_) {
      dispatch_apply(final_image_->GetWidth(), loop_dispactch_queue_, ^(size_t x_) {
        uint32_t x = (uint32_t)x_;
        uint32_t y = (uint32_t)y_;
        
        uint32_t pixel_idx = x + y * final_image_->GetWidth();
        glm::vec4 pixel = PerPixel(x, y);
        
        pixel = glm::clamp(pixel, glm::vec4(0.0f), glm::vec4(1.0f));
        image_data_[pixel_idx] = ConevrtToRgba(pixel);
      });
    });
    final_image_->SetData(image_data_);
  }

  glm::vec4 RendererLayer::PerPixel(uint32_t x, uint32_t y) {
    Ray ray;
    ray.origin = editor_camera_.GetPosition();
    
    uint32_t pixel_idx = x + y * final_image_->GetWidth();
    ray.direction = editor_camera_.GetRayDirections()[pixel_idx];
    
    glm::vec3 color(0.0f);
    float multiplier = 1.0f;
    int32_t bounces = 5;
    for (uint32_t i = 0; i < bounces; i++) {
      HitPayload payload = TraceRay(ray);
      if (payload.hit_distance < 0) {
        glm::vec3 sky_color = glm::vec3(0.6, 0.7, 0.9);
        color += sky_color * multiplier;
        break;
      }
      
      glm::vec3 light_direction = glm::normalize(glm::vec3(-1, -1, -1));
      float light_intensity = glm::max(glm::dot(payload.world_normal, -light_direction), 0.0f); // cos(angle);
      
      const Sphere& sphere = spheres[payload.object_idx];
      glm::vec3 sphere_color = sphere.albedo;
      sphere_color *= light_intensity;
      
      color += sphere_color * multiplier;
      
      // Decreasing the energy
      multiplier *= 0.5f;
      
      // Origin is now hit position (Reflection point)
      // To avoid the new pay position to start from actual sphere we shift the
      // origin with the help of normal but very less
      ray.origin = payload.world_position + payload.world_normal * 0.0001f;
      
      float r_pos = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
      r_pos -= 0.5f;
      
      ray.direction = glm::reflect(ray.direction,
                                   payload.world_normal + (float)(0.1) * glm::vec3(r_pos));
    }
    
    return glm::vec4(color, 1.0f);
  }
  
  HitPayload RendererLayer::TraceRay(const Ray& ray) {
    int32_t closest_sphere_idx = -1;
    float hit_distance = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < spheres.size(); i++) {
      const Sphere& sphere = spheres[i];
      
      if (sphere.Hit(ray, editor_camera_.GetNear(), hit_distance)) {
        closest_sphere_idx = (int32_t)i;
      } else {
        continue;
      }
    }
    
    if (closest_sphere_idx < 0)
      return Miss(ray);
    
    return ClosestHit(ray, hit_distance, closest_sphere_idx);
  }
  
  HitPayload RendererLayer::ClosestHit(const Ray& ray, float hit_distance, int32_t object_idx) {
    HitPayload payload;
    payload.hit_distance = hit_distance;
    payload.object_idx = object_idx;
    
    const Sphere closest_sphere = spheres[object_idx];
    glm::vec3 origin = ray.origin - closest_sphere.position;
    payload.world_position = origin + (ray.direction * hit_distance);
    payload.world_normal = glm::normalize(payload.world_position);
    
    // Move back to origin
    payload.world_position += closest_sphere.position;
    
    return payload;
  }
  
  HitPayload RendererLayer::Miss(const Ray& ray) {
    HitPayload payload;
    payload.hit_distance = -1;
    return payload;
  }
    
  void RendererLayer::Update(Timestep ts) {
    if (editor_camera_.Update(ts)) {
      
    }

    editor_camera_.SetViewportSize(viewport_width_, viewport_height_);
    Resize();
    
    Render();
  }
  
  void RendererLayer::EventHandler(Event& event) {
//    editor_camera_.EventHandler(event);
  }
  
  void RendererLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
  
    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("RayTrace Viewport");
    ImGui::PushID("RayTrace Viewport");
    
    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    viewport_width_ = (uint32_t) viewport_panel_size.x;
    viewport_height_ = (uint32_t) viewport_panel_size.y;

    ImGui::Image(INT2VOIDP(final_image_->GetRendererID()),
                 viewport_panel_size,
                 ImVec2{ 0, 1 },
                 ImVec2{ 1, 0 });
    
    ImGui::PopStyleVar();
    
    ImGui::PopID();
    ImGui::End();
    
    ImguiAPI::EndDcocking();
    
  }
  
}

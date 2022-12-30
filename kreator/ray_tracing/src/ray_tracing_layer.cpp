//
//  ray_tracing_layer.cpp
//  kreator
//
//  Created by Ashish . on 30/12/22.
//

#include "ray_tracing_layer.hpp"
#include <dispatch/dispatch.h>

namespace ray_tracing {
  
  dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

  static uint32_t ConevrtToRgba(const glm::vec4& pixel) {
    uint8_t r = uint8_t(pixel.r * 255.0f);
    uint8_t g = uint8_t(pixel.g * 255.0f);
    uint8_t b = uint8_t(pixel.b * 255.0f);
    uint8_t a = uint8_t(pixel.a * 255.0f);
    return ( (a << 24) | (b << 16) | (g << 8) | r);
  }
  
  RayTracingLayer::RayTracingLayer() : Layer("Kreator") {
    IK_INFO("Creating RayTracing Layer instance ... ");
  }
  
  RayTracingLayer::~RayTracingLayer() {
    IK_WARN("Destroying RayTracing Layer instance !!! ");
  }
  
  void RayTracingLayer::Attach() {
    IK_INFO("Attaching RayTracing Layer instance");
  }
  
  void RayTracingLayer::Detach() {
    IK_WARN("Detaching RayTracing Layer instance ");
  }
  
  void RayTracingLayer::Resize() {
    if (final_image_) {
      // No resize
      if (final_image_->GetWidth() == viewport_width_ and final_image_->GetHeight() == viewport_height_)
        return;
      
      final_image_->Resize(viewport_width_, viewport_height_);
    }
    else {
      final_image_ = Image::Create(viewport_width_, viewport_height_, TextureFormat::RGBA);
    }
    
    delete[] image_data_ ;
    image_data_ = new uint32_t[viewport_width_ * viewport_height_];

  }
  
  void RayTracingLayer::Render() {
    const glm::vec3& ray_origin = editor_camera_.GetPosition();

    dispatch_apply(final_image_->GetHeight(), queue, ^(size_t y) {
      dispatch_apply(final_image_->GetWidth(), queue, ^(size_t x) {
        uint32_t pixel_idx = (uint32_t)x + (uint32_t)y * final_image_->GetWidth();
        const glm::vec3& ray_direction = editor_camera_.GetRayDirections()[pixel_idx];
        Ray ray;
        ray.origin = ray_origin;
        ray.direction = ray_direction;
        
        glm::vec4 pixel = TraceRay(ray);
        pixel = glm::clamp(pixel, glm::vec4(0.0f), glm::vec4(1.0f));

        image_data_[pixel_idx] = ConevrtToRgba(pixel);
      });
    });
    final_image_->SetData(image_data_);
  }
  
  glm::vec4 RayTracingLayer::TraceRay(const Ray& ray) {
    float radius = 0.5f;
    
    //       at^2       +       bt        +       c             = 0
    // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2 + ay^2 - r^2) = 0
    // where,
    //    a : Ray Origin
    //    b : Direction of Ray
    //    r : Radius of Cirlce/Shphere
    //    t : Distance of point on ray from 'a'
    
    // float a = ray_direction.x * ray_direction + ray_direction.y * ray_direction.y + ray_direction.z * ray_direction.z;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(ray.origin, ray.direction);
    float c = glm::dot(ray.origin, ray.origin) - (radius * radius);

    // Discriminant
    // b^2 -4ac
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) {
      return glm::vec4(0, 0, 0, 1);
    }

    float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
    float closest_t = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    
    glm::vec3 hit_point = ray.origin + (ray.direction * closest_t);
    glm::vec3 normal = glm::normalize(hit_point);
    
    glm::vec3 light_direction = glm::normalize(glm::vec3(-1, -1, -1));
    
    float dot = glm::max(glm::dot(normal, -light_direction), 0.0f); // cos(angle);

    glm::vec3 sphere_color(1, 0, 1);
    sphere_color *= dot;
    return glm::vec4(sphere_color, 1.0f);
  }
  
  void RayTracingLayer::Update(Timestep ts) {
    editor_camera_.Update(ts);
    editor_camera_.SetViewportSize(viewport_width_, viewport_height_);

    Resize();
    Render();
    Renderer::Clear({0.2, 0.4, 0.2, 1.0});
  }
  
  void RayTracingLayer::EventHandler(Event& event) {
  }
  
  void RayTracingLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();

    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Kreator Viewport");
    ImGui::PushID("Kreator Viewport");
    
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

//
//  ray_tracing_layer.cpp
//  kreator
//
//  Created by Ashish . on 30/12/22.
//

#include "ray_tracing_layer.hpp"

namespace ray_tracing {
  
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
    scene_.shperes.push_back(Sphere({0, 0, 0}, 0.5, {1, 0, 1}));
    scene_.shperes.push_back(Sphere({0, 0, -5}, 0.5, {0, 0, 1}));
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

    dispatch_apply(final_image_->GetHeight(), loop_dispactch_queue_, ^(size_t y) {
      dispatch_apply(final_image_->GetWidth(), loop_dispactch_queue_, ^(size_t x) {
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
    if (scene_.shperes.size() == 0) {
      return glm::vec4(0, 0, 0, 1);
    }

    const Sphere* closest_sphere = nullptr;
    float hit_distance = std::numeric_limits<float>::max();
    for (const Sphere& sphere : scene_.shperes) {
      glm::vec3 origin = ray.origin - sphere.position;
      
      // float a = ray_direction.x * ray_direction + ray_direction.y * ray_direction.y + ray_direction.z * ray_direction.z;
      float a = glm::dot(ray.direction, ray.direction);
      float b = 2.0f * glm::dot(origin, ray.direction);
      float c = glm::dot(origin, origin) - (sphere.radius * sphere.radius);
      
      // Discriminant
      // b^2 -4ac
      float discriminant = b * b - 4.0f * a * c;
      if (discriminant < 0) {
        continue;
      }
      
      // -b +- sqrt(discriminant) / 2a
      float closest_t = (-b - glm::sqrt(discriminant)) / (2.0f * a);
      // Second hit currently unused
      // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
      if (closest_t < hit_distance) {
        hit_distance = closest_t;
        closest_sphere = &sphere;
      }
    }
    
    if (!closest_sphere)
      return glm::vec4(0, 0, 0, 1);
    
    glm::vec3 origin = ray.origin - closest_sphere->position;
    glm::vec3 hit_point = origin + (ray.direction * hit_distance);
    
    glm::vec3 normal = glm::normalize(hit_point);
    
    glm::vec3 light_direction = glm::normalize(glm::vec3(-1, -1, -1));
    
    float dot = glm::max(glm::dot(normal, -light_direction), 0.0f); // cos(angle);

    glm::vec3 sphere_color = closest_sphere->albedo;
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
    editor_camera_.EventHandler(event);
  }
  
  void RayTracingLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();
    
    {
      ImGui::Begin("Scene Sphere");
      ImGui::PushID("Scene Sphere");
      
      for (size_t i = 0; i < scene_.shperes.size(); i++) {
        ImGui::PushID((uint32_t)i);
        ImGui::DragFloat3("position", glm::value_ptr(scene_.shperes[i].position), 0.1f);
        ImGui::DragFloat("radius", &scene_.shperes[i].radius, 0.1);
        ImGui::ColorEdit3("color", glm::value_ptr(scene_.shperes[i].albedo));
        ImGui::Separator();
        ImGui::PopID();
      }
      
      ImGui::PopID();
      ImGui::End();
    }

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

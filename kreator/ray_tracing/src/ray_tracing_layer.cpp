//
//  ray_tracing_layer.cpp
//  kreator
//
//  Created by Ashish . on 30/12/22.
//

#include "ray_tracing_layer.hpp"

namespace ray_tracing {
  
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
    for (uint32_t y = 0; y < final_image_->GetHeight(); y++) {
      for (uint32_t x = 0; x < final_image_->GetWidth(); x++) {
        glm::vec2 coords = { (float)x / (float)final_image_->GetWidth(), (float)y / (float)final_image_->GetHeight() };
        coords = coords * 2.0f - 1.0f; // Map [-1 : 1]
        image_data_[x + y * final_image_->GetWidth()] = PerPixel(coords);
      }
    }
    final_image_->SetData(image_data_);
  }
  
  uint32_t RayTracingLayer::PerPixel(const glm::vec2& coord) {
    glm::vec3 ray_origin(0.0f, 0.0f, -2.0f);
    
    // For simple use -1 as Z
    glm::vec3 ray_direction = { coord.x, coord.y, -1 };
    // ray_direction = glm::normalize(ray_direction);
    
    float radius = 0.5f;
    
    //       at^2       +       bt        +       c             = 0
    // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2 + ay^2 - r^2) = 0
    // where,
    //    a : Ray Origin
    //    b : Direction of Ray
    //    r : Radius of Cirlce/Shphere
    //    t : Distance of point on ray from 'a'
    
    // float a = ray_direction.x * ray_direction + ray_direction.y * ray_direction.y + ray_direction.z * ray_direction.z;
    float a = glm::dot(ray_direction, ray_direction);
    float b = 2.0f * glm::dot(ray_origin, ray_direction);
    float c = glm::dot(ray_origin, ray_origin) - (radius*radius);
    
    // Discriminant
    // b^2 -4ac
    float discriminant = b * b - 4.0f * a * c;
    
    if (discriminant > 0) {
      return 0xffff00ff;
    }
    else {
      return 0;
    }
  }
  
  void RayTracingLayer::Update(Timestep ts) {
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

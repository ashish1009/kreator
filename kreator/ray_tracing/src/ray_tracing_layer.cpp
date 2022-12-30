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
  
  void RayTracingLayer::Update(Timestep ts) {
    if (!image_) {
      image_ = Image::Create(800, 800, TextureFormat::RGBA);
      delete[] image_data_ ;
      image_data_ = new uint32_t[viewport_width_ * viewport_height_];
    }
    
    for (uint32_t i = 0; i < viewport_width_ * viewport_height_; i++) {
      image_data_[i] = 1;
    }
    
    image_->SetData(image_data_);

    Renderer::ResetStatsEachFrame();
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
    ImGui::Image(INT2VOIDP(image_->GetRendererID()),
                 viewport_panel_size,
                 ImVec2{ 0, 1 },
                 ImVec2{ 1, 0 });
    
    ImGui::PopStyleVar();
    
    ImGui::PopID();
    ImGui::End();
    
    ImguiAPI::EndDcocking();

  }
  
} 

//
//  renderer_layer.cpp
//  kreator
//
//  Created by Ashish . on 31/12/22.
//

#include "renderer_layer.hpp"

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
    }
    else {
      final_image_ = Image::Create(viewport_width_, viewport_height_, TextureFormat::RGBA);
      delete[] image_data_ ;
      image_data_ = new uint32_t[viewport_width_ * viewport_height_];
    }
  }
  
  void RendererLayer::Render() {
    dispatch_apply(final_image_->GetHeight(), loop_dispactch_queue_, ^(size_t y) {
      dispatch_apply(final_image_->GetWidth(), loop_dispactch_queue_, ^(size_t x) {
        uint32_t pixel_idx = (uint32_t)x + (uint32_t)y * final_image_->GetWidth();
        
        float rand_val = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        rand_val -= 0.5f;
        glm::vec4 pixel = glm::vec4(rand_val);
        pixel = glm::clamp(pixel, glm::vec4(0.0f), glm::vec4(1.0f));
        image_data_[pixel_idx] = ConevrtToRgba(pixel);
      });
    });
    final_image_->SetData(image_data_);
  }
  
  void RendererLayer::Update(Timestep ts) {
    if (editor_camera_.Update(ts)) {
      
    }

    editor_camera_.SetViewportSize(viewport_width_, viewport_height_);
    Resize();
    
    Render();
  }
  
  void RendererLayer::EventHandler(Event& event) {
    editor_camera_.EventHandler(event);
  }
  
  void RendererLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();
  
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

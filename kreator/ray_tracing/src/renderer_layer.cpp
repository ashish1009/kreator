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
    
//    editor_camera_.SetPosition({0, 0, 6});
//    int32_t num_spheres = 20;
//    spheres.resize(num_spheres);
//    for (int32_t i = 0; i < num_spheres; i++) {
//      spheres[i].position = {0, 0, 0};
//      spheres[i].radius = 1.0f;
//    }
    
    spheres.push_back(Sphere({0, 0.0, 0}, 1));
    spheres.push_back(Sphere({0, -101, 0}, 100));
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
  
  glm::vec3 RendererLayer::RayColor(const Ray& ray, int32_t bounce) {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (bounce <= 0)
      return glm::vec3(0.0f);

    HitPayload payload;
    if (TraceRay(ray, payload)) {
      Ray reflected_ray;
      // Origin is now hit position (Reflection point)
      // To avoid the new pay position to start from actual sphere we shift the
      // origin with the help of normal but very less
      reflected_ray.origin = payload.world_position + payload.world_normal * 0.0001f;
      reflected_ray.direction = glm::reflect(ray.direction,
                                             payload.world_normal + Math::RandomInUnitSphere());
      return float(0.5) * RayColor(reflected_ray, bounce - 1);
    } else {
      glm::vec3 unit_direction = ray.direction;
      float hit_point = 0.5 * (unit_direction.y + 1.0);
      return (((float)1.0 - hit_point) * glm::vec3(1.0, 1.0, 1.0)) + (hit_point * glm::vec3(0.5, 0.7, 1.0));
    }
  }

  glm::vec4 RendererLayer::PerPixel(uint32_t x, uint32_t y) {
    uint32_t pixel_idx = x + y * final_image_->GetWidth();
      
    Ray ray;
    ray.origin = editor_camera_.GetPosition();
    ray.direction = editor_camera_.GetRayDirections().at(pixel_idx);

    return glm::vec4(RayColor(ray, 10), 1.0f);
  }
  
  bool RendererLayer::TraceRay(const Ray& ray, HitPayload& payload) {
    HitPayload temp_payload = payload;
    bool hit_anything = false;
    float hit_distance = std::numeric_limits<float>::max();

    for (int32_t i = 0; i < spheres.size(); i++) {
      if (spheres[i].Hit(ray, editor_camera_.GetNear(), hit_distance, temp_payload)) {
        hit_anything = true;
        hit_distance = temp_payload.hit_distance;
        payload = temp_payload;
      }
    }
    
    return hit_anything;
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

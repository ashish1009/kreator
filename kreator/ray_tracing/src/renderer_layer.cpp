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

    {
      Material& mat = materials.emplace_back(Material());
      mat.albedo = {0.7, 0.3, 0.};
      mat.type = Material::Type::Lambertian;
    }
    {
      Material& mat = materials.emplace_back(Material());
      mat.albedo = {0.8, 0.8, 0.8};
      mat.type = Material::Type::Dielectric;
      mat.refractive_index = 1.5f;
    }
    {
      Material& mat = materials.emplace_back(Material());
      mat.albedo = {0.8, 0.6, 0.2};
      mat.type = Material::Type::Metal;
      mat.fuzz = 0.0f;
    }
    {
      Material& mat = materials.emplace_back(Material());
      mat.albedo = {0.8, 0.8, 0.0};
      mat.type = Material::Type::Lambertian;
    }
    {
      Sphere sphere;
      sphere.position = {0.0f, 0.0f, 0.0f};
      sphere.radius = 1.0f;
      sphere.material_index = 0;
      
      spheres.push_back(sphere);
    }
    {
      Sphere sphere;
      sphere.position = {-2.0f, 0.0f, 0.0f};
      sphere.radius = 1.0f;
      sphere.material_index = 1;
      
      spheres.push_back(sphere);
    }
    {
      Sphere sphere;
      sphere.position = {2.0f, 0.0f, 0.0f};
      sphere.radius = 1.0f;
      sphere.material_index = 2;
      
      spheres.push_back(sphere);
    }
    {
      Sphere sphere;
      sphere.position = {0.0f, -101.0f, 0.0f};
      sphere.radius = 100.0f;
      
      sphere.material_index = 3;
      spheres.push_back(sphere);
    }
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

      delete[] accumulation_data_ ;
      accumulation_data_ = new glm::vec4[viewport_width_ * viewport_height_];

    } else {
      final_image_ = Image::Create(viewport_width_, viewport_height_, TextureFormat::RGBA);
      delete[] image_data_ ;
      image_data_ = new uint32_t[viewport_width_ * viewport_height_];

      delete[] accumulation_data_ ;
      accumulation_data_ = new glm::vec4[viewport_width_ * viewport_height_];
    }
  }
  
  void RendererLayer::Render() {
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

  glm::vec4 RendererLayer::PerPixel(uint32_t x, uint32_t y) {
    glm::vec3 color(0.0f);
#if 1
#else 
    Ray ray;
    ray.origin = editor_camera_.GetPosition();
    
    uint32_t pixel_idx = x + y * final_image_->GetWidth();
    ray.direction = editor_camera_.GetRayDirections()[pixel_idx];
    
    float multiplier = 1.0f;
    int32_t bounces = 5;
    for (uint32_t i = 0; i < bounces; i++) {
      HitPayload payload = TraceRay(ray);
      if (payload.hit_distance < 0) {
        glm::vec3 sky_color = glm::vec3(0.6, 0.7, 0.9);
        color += sky_color * multiplier;
        break;
      }
      
      const Sphere& sphere = spheres[payload.object_idx];
      const Material& material = materials[sphere.material_index];
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
    
#endif
    return glm::vec4(color, 1.0f);
  }
  
  HitPayload RendererLayer::TraceRay(const Ray& ray) {
    int32_t closest_sphere_idx = -1;
    float hit_distance = std::numeric_limits<float>::max();

    for (size_t i = 0; i < spheres.size(); i++) {
      const Sphere& sphere = spheres[i];
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
  
  HitPayload RendererLayer::ClosestHit(const Ray& ray, float hit_distance, int32_t object_idx) {
    HitPayload payload;
    payload.hit_distance = hit_distance;
    payload.object_idx = object_idx;
    
    const Sphere closest_sphere = spheres[object_idx];
    glm::vec3 origin = ray.origin - closest_sphere.position;
    payload.world_position = origin + (ray.direction * hit_distance);
    payload.world_normal = glm::normalize(payload.world_position);
    payload.SetFaceNormal(ray);
    
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
      frame_index_ = 1;
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
    
    {
      ImGui::Begin("Scene Sphere");
      ImGui::PushID("Scene Sphere");
      
      for (size_t i = 0; i < spheres.size(); i++) {
        ImGui::PushID((uint32_t)i);
        ImGui::DragFloat3("position", glm::value_ptr(spheres[i].position), 0.1f);
        ImGui::DragFloat("radius", &spheres[i].radius, 0.1, 0.0);
        ImGui::DragInt("material", &spheres[i].material_index, 1., 0.0, (int)(materials.size() - 1));
        ImGui::Separator();
        ImGui::PopID();
      }
      
      ImGui::Separator();
      ImGui::Separator();
      
      for (size_t i = 0; i < materials.size(); i++) {
        ImGui::PushID((uint32_t)i);
        ImGui::ColorEdit3("color", glm::value_ptr(materials[i].albedo));
        if (materials[i].type == Material::Type::Metal)
          ImGui::DragFloat("fuzz", &materials[i].fuzz, 0.001, 0.0, 1.0);
        if (materials[i].type == Material::Type::Dielectric)
          ImGui::DragFloat("ri", &materials[i].refractive_index, 0.001, 0.0, 1.0);
        ImGui::Separator();
        ImGui::PopID();
      }
      
      ImGui::Checkbox("Accumulate", &setting_.accumulate);
      
      if (ImGui::Button("Reset")) {
        frame_index_ = 1;
      }
      if (ImGui::Button("Render")) {
        setting_.render = setting_.render ? false : true;
      }
            
      ImGui::PopID();
      ImGui::End();
    }
  
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

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
    {
      Material& pink_mat = scene_.materials.emplace_back(Material());
      pink_mat.albedo = {1.0f, 0.0f, 1.0f};
      pink_mat.roughness = 0.0f;
    }
    {
      Material& blue_mat = scene_.materials.emplace_back(Material());
      blue_mat.albedo = {0.0f, 0.0f, 1.0f};
      blue_mat.roughness = 0.1f;
    }
    {
      Sphere sphere;
      sphere.position = {0.0f, 0.0f, 0.0f};
      sphere.radius = 1.0f;
      sphere.material_index = 0;
      
      scene_.shperes.push_back(sphere);
    }
    {
      Sphere sphere;
      sphere.position = {0.0f, -101.0f, 0.0f};
      sphere.radius = 100.0f;
      
      sphere.material_index = 1;
      scene_.shperes.push_back(sphere);
      ///Users/ashish./Downloads/obj_raytracer-master/obj/dino.obj
    }//    /Users/ashish./iKan_storage/Github/Projects/ikan_ws/kreator/editor/editor_assets/model/static/cyborg/cyborg.obj
    scene.Add(std::make_shared<TriangleMesh>("/Users/ashish./Downloads/obj_raytracer-master/obj/dino.obj"));
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
    delete[] accumulation_data_ ;
    accumulation_data_ = new glm::vec4[viewport_width_ * viewport_height_];
  }
  
  Matrix4x4f c2w = Matrix4x4f(1.0f, 0.0f, 0.0f, 0.0f,
                              0.0,  1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 10.0f, 50.0f, 1.0f);
  
  Ray RayTracingLayer::get_ray(float u, float v) {
    
    float aspect_ratio = viewport_width_ / viewport_height_;
    float scale = tan(deg2rad(70.0f * 0.5f));

    // Maya-style
    float ndc_x = (2.0f * (u + 0.5f) / (float)viewport_width_ - 1.0f) * scale;
    float ndc_y = (1.0f - 2.0f * (v + 0.5f) / (float)viewport_height_) * scale * 1.0f / aspect_ratio;
    
    
    Ray r;
    Vector3f orig = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f dir =  glm::normalize(Vector3f(ndc_x, ndc_y, -1.0f));
    
    r.origin = TransformPointMatrix(c2w, orig);
    r.direction = TransformDirMatrix(c2w, dir);
    r.far_plane = 9999.9f;
    r.near_plane = 0.001f;
    
    
    return r;
  }
  
  glm::vec3 RayTracingLayer::CastRay(const Ray& r, const Shape& scene) {
    SurfaceInteraction interaction;

    if (scene.Intersect(r, interaction)) {
      //return Vector3f(glm::dot(interaction.Ng, -r.d));
      return glm::vec3(interaction.AOV);
    }
    
    return glm::vec3(0.18f);

  }
  
  void RayTracingLayer::Render() {
    if (frame_index_ == 1) {
      memset(accumulation_data_, 0, final_image_->GetWidth() * final_image_->GetHeight());
    }

    dispatch_apply(final_image_->GetHeight(), loop_dispactch_queue_, ^(size_t y) {
      dispatch_apply(final_image_->GetWidth(), loop_dispactch_queue_, ^(size_t x) {
        uint32_t pixel_idx = (uint32_t)x + (uint32_t)y * final_image_->GetWidth();
    
#define MeshRay 1
#if MeshRay
        Ray ray = get_ray(x, y);
        Vector3f colour = CastRay(ray, scene);
        
        image_data_[pixel_idx] = ConevrtToRgba(glm::vec4(colour, 1.0f));
#else
        glm::vec4 pixel = PerPixel((uint32_t)x, (uint32_t)y);
        if (frame_index_ == 1)
          accumulation_data_[pixel_idx] = pixel;
        else
          accumulation_data_[pixel_idx] += pixel;

        glm::vec4 accumulated_color = accumulation_data_[pixel_idx];
        accumulated_color /= (float)frame_index_;

        accumulated_color = glm::clamp(accumulated_color, glm::vec4(0.0f), glm::vec4(1.0f));
        image_data_[pixel_idx] = ConevrtToRgba(accumulated_color);
        
#endif
      });
    });
    final_image_->SetData(image_data_);
    
    if (setting_.accumulate) {
      frame_index_++;
    }
    else {
      frame_index_ = 1;
    }
  }
  
  glm::vec4 RayTracingLayer::PerPixel(uint32_t x, uint32_t y) {
    Ray ray;
    ray.origin = editor_camera_.GetPosition();
    
    uint32_t pixel_idx = x + y * final_image_->GetWidth();
    ray.direction = editor_camera_.GetRayDirections().at(pixel_idx);
    
    glm::vec3 color(0.0f);
    float multiplier = 1.0f;
    int32_t bounces = 15;
    for (uint32_t i = 0; i < bounces; i++) {
      RayTracingLayer::HitPayload payload = TraceRay(ray);
      if (payload.hit_distance < 0) {
        glm::vec3 sky_color = glm::vec3(0.6, 0.7, 0.9);
        color += sky_color * multiplier;
        break;
      }
      
      glm::vec3 light_direction = glm::normalize(glm::vec3(-1, -1, -1));
      float light_intensity = glm::max(glm::dot(payload.world_normal, -light_direction), 0.0f); // cos(angle);
      
      const Sphere& sphere = scene_.shperes[payload.object_idx];
      const Material& material = scene_.materials[sphere.material_index];
      glm::vec3 sphere_color = material.albedo;
      sphere_color *= light_intensity;
      
      color += sphere_color * multiplier;
      
      multiplier *= 0.5f;

      // Origin is now hit position (Reflection point)
      // To avoid the new pay position to start from actual sphere we shift the
      // origin with the help of normal but very less
      ray.origin = payload.world_position + payload.world_normal * 0.0001f;
      
      float r_pos = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
      r_pos -= 0.5f;
      
      ray.direction =  glm::reflect(ray.direction,
                                    payload.world_normal + material.roughness * glm::vec3(r_pos));
    }
    
    return glm::vec4(color, 1.0f);
  }
  
  RayTracingLayer::HitPayload RayTracingLayer::TraceRay(const Ray& ray) {
    int32_t closest_sphere_idx = -1;
    float hit_distance = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < scene_.shperes.size(); i++) {
      const Sphere& sphere = scene_.shperes[i];
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
      if (closest_t > 0.0f and closest_t < hit_distance) {
        hit_distance = closest_t;
        closest_sphere_idx = (int32_t)i;
      }
    }
    
    if (closest_sphere_idx < 0)
      return Miss(ray);
    
    return ClosestHit(ray, hit_distance, closest_sphere_idx);
  }
  
  RayTracingLayer::HitPayload RayTracingLayer::ClosestHit(const Ray& ray, float hit_distance, int32_t object_idx) {
    RayTracingLayer::HitPayload payload;
    payload.hit_distance = hit_distance;
    payload.object_idx = object_idx;

    const Sphere closest_sphere = scene_.shperes[object_idx];
    glm::vec3 origin = ray.origin - closest_sphere.position;
    payload.world_position = origin + (ray.direction * hit_distance);
    payload.world_normal = glm::normalize(payload.world_position);

    // Move back to origin
    payload.world_position += closest_sphere.position;
    
    return payload;
  }

  RayTracingLayer::HitPayload RayTracingLayer::Miss(const Ray& ray) {
    RayTracingLayer::HitPayload payload;
    payload.hit_distance = -1;
    return payload;
  }

  void RayTracingLayer::Update(Timestep ts) {
    if (editor_camera_.Update(ts))
      frame_index_ = 1;

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
        ImGui::DragFloat("radius", &scene_.shperes[i].radius, 0.1, 0.0);
        ImGui::DragInt("material", &scene_.shperes[i].material_index, 1., 0.0, (int)(scene_.materials.size() - 1));
        ImGui::Separator();
        ImGui::PopID();
      }
      
      ImGui::Separator();
      ImGui::Separator();
      
      for (size_t i = 0; i < scene_.materials.size(); i++) {
        ImGui::PushID((uint32_t)i);
        ImGui::ColorEdit3("color", glm::value_ptr(scene_.materials[i].albedo));
        ImGui::DragFloat("roughness", &scene_.materials[i].roughness, 0.05, 0.0, 1.0);
        ImGui::DragFloat("matellic", &scene_.materials[i].metallic, 0.05, 0.0, 1.0);
        ImGui::Separator();
        ImGui::PopID();
      }

      ImGui::Checkbox("Accumulate", &setting_.accumulate);
      
      if (ImGui::Button("Reset")) {
        frame_index_ = 1;
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

//
//  renderer_layer.cpp
//  kreator
//
//  Created by Ashish . on 31/12/22.
//

#include "renderer_layer.hpp"

namespace ray_tracing {

  RendererLayer::RendererLayer() : Layer("Ray Trace Renderer Layer") {
    IK_INFO("Ray Tracing", "Creating Ray Trace Renderer Layer instance ... ");
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN("Ray Tracing", "Destroying Ray Trace Renderer Layer instance !!! ");
  }
  
  void RendererLayer::Attach() {
    IK_INFO("Ray Tracing", "Attaching Ray Trace Renderer Layer instance");
    
    editor_camera_.SetPosition({0, 0, 6});
//
//    {
//      Material& mat = scene_.materials.emplace_back(Material());
//      mat.albedo = {0.7, 0.3, 0.};
//      mat.type = Material::Type::Lambertian;
//    }
//    {
//      Material& mat = scene_.materials.emplace_back(Material());
//      mat.albedo = {0.8, 0.8, 0.8};
//      mat.type = Material::Type::Dielectric;
//      mat.refractive_index = 1.0f;
//    }
//    {
//      Material& mat = scene_.materials.emplace_back(Material());
//      mat.albedo = {0.8, 0.6, 0.2};
//      mat.type = Material::Type::Metal;
//      mat.fuzz = 0.0f;
//    }
//    {
//      Material& mat = scene_.materials.emplace_back(Material());
//      mat.albedo = {0.8, 0.8, 0.0};
//      mat.type = Material::Type::Lambertian;
//    }
//    {
//      Sphere sphere;
//      sphere.position = {0.0f, 1.0f, 0.0f};
//      sphere.radius = 2.0f;
//      sphere.material_index = 2;
//      
//      scene_.spheres.push_back(sphere);
//    }
//    {
//      Sphere sphere;
//      sphere.position = {-5.0f, 1.0f, 0.0f};
//      sphere.radius = 2.0f;
//      sphere.material_index = 1;
//
//      scene_.spheres.push_back(sphere);
//    }
//    {
//      Sphere sphere;
//      sphere.position = {5.0f, 1.0f, 5.0f};
//      sphere.radius = 2.0f;
//      sphere.material_index = 0;
//
//      scene_.spheres.push_back(sphere);
//    }
//    {
//      Sphere sphere;
//      sphere.position = {-7.0f, 0.0f, 3.0f};
//      sphere.radius = 1.0f;
//      sphere.material_index = 2;
//
//      scene_.spheres.push_back(sphere);
//    }
//    {
//      Sphere sphere;
//      sphere.position = {-2.0f, 0.0f, 4.0f};
//      sphere.radius = 1.0f;
//      sphere.material_index = 1;
//
//      scene_.spheres.push_back(sphere);
//    }
//    {
//      Sphere sphere;
//      sphere.position = {1.0f, 0.0f, 7.0f};
//      sphere.radius = 1.0f;
//      sphere.material_index = 0;
//
//      scene_.spheres.push_back(sphere);
//    }
//    {
//      Sphere sphere;
//      sphere.position = {0.0f, -101.0f, 0.0f};
//      sphere.radius = 100.0f;
//      
//      sphere.material_index = 3;
//      scene_.spheres.push_back(sphere);
//    }
  }
  
  void RendererLayer::Detach() {
    IK_WARN("Ray Tracing", "Detaching Ray Trace Renderer Layer instance ");
  }
    
  void RendererLayer::Update(Timestep ts) {
    if (editor_camera_.Update(ts)) {
      renderer_.ResetFrameIndex();
    }

    editor_camera_.SetViewportSize(viewport_width_, viewport_height_);
    renderer_.Resize(viewport_width_, viewport_height_);
    
    renderer_.Render(scene_, editor_camera_);
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
      
      for (size_t i = 0; i < scene_.spheres.size(); i++) {
        ImGui::PushID((uint32_t)i);
        ImGui::DragFloat3("position", glm::value_ptr(scene_.spheres[i].position), 0.1f);
        ImGui::DragFloat("radius", &scene_.spheres[i].radius, 0.1, 0.0);
        ImGui::DragInt("material", &scene_.spheres[i].material_index, 1., 0.0, (int)(scene_.materials.size() - 1));
        ImGui::Separator();
        ImGui::PopID();
      }
      
      ImGui::Separator();
      ImGui::Separator();
      
      for (size_t i = 0; i < scene_.materials.size(); i++) {
        ImGui::PushID((uint32_t)i);
        ImGui::ColorEdit3("color", glm::value_ptr(scene_.materials[i].albedo));
        if (scene_.materials[i].type == Material::Type::Metal)
          ImGui::DragFloat("fuzz", &scene_.materials[i].fuzz, 0.01, 0.0, 1.0);
        if (scene_.materials[i].type == Material::Type::Dielectric)
          ImGui::DragFloat("ri", &scene_.materials[i].refractive_index, 0.01, 0.0);
        ImGui::Separator();
        ImGui::PopID();
      }
      
      ImGui::Checkbox("Accumulate", &renderer_.GetSetting().accumulate);
      
      if (ImGui::Button("Reset")) {
        renderer_.ResetFrameIndex();
      }
      if (ImGui::Button("Render")) {
        renderer_.GetSetting().render = renderer_.GetSetting().render ? false : true;
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

    ImGui::Image(INT2VOIDP(renderer_.GetFinalImage()->GetRendererID()),
                 viewport_panel_size,
                 ImVec2{ 0, 1 },
                 ImVec2{ 1, 0 });
    
    ImGui::PopStyleVar();
    
    ImGui::PopID();
    ImGui::End();
    
    ImguiAPI::EndDcocking();
    
  }
  
}

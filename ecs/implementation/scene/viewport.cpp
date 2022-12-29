//
//  viewport.cpp
//  ecs
//
//  Created by Ashish . on 28/12/22.
//

#include "viewport.hpp"

namespace ecs {
  
  Viewport::Viewport(const glm::vec4& fb_color) {
    FrameBuffer::Specification fb_spec;
    fb_spec.color = fb_color;
    fb_spec.attachments = {
      FrameBuffer::Attachments::TextureFormat::RGBA8,
      FrameBuffer::Attachments::TextureFormat::Depth24Stencil,
    };
    
    framebuffer = FrameBuffer::Create(fb_spec);
  }
  
  void Viewport::UpdateMousePos() {
    auto [mx, my] = ImGui::GetMousePos();
    mx -= bounds[0].x;
    my -= bounds[0].y;
    
    my = height - my;
    
    mouse_pos_x = (int32_t)mx;
    mouse_pos_y = (int32_t)my;
  }
  
  void Viewport::UpdateBound() {
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 min_bound   = ImGui::GetWindowPos();
    
    min_bound.x += cursor_pos.x;
    min_bound.y += cursor_pos.y;
    
    ImVec2 max_bound = { min_bound.x + window_size.x, min_bound.y + window_size.y };
    bounds[0] = { min_bound.x, min_bound.y };
    bounds[1] = { max_bound.x, max_bound.y };
  }
  
  bool Viewport::IsFramebufferResized() {
    const FrameBuffer::Specification& spec = framebuffer->GetSpecification();
    
    return
    width > 0 and
    height > 0 and // zero sized framebuffer is invalid
    (spec.width != width or spec.height != height);
  }

  void Viewport::RenderGui() {
    ImGui::Begin("Viewport Data",nullptr, ImGuiWindowFlags_NoScrollbar);
    ImGui::PushID("Viewport Data");

    ImGui::Columns(5);
    ImGui::SetColumnWidth(0, 90);
    ImGui::Text("%d x %d", mouse_pos_x, mouse_pos_y);
    ImGui::NextColumn();
    
    ImGui::SetColumnWidth(1, 90);
    if (focused)
      ImGui::Text("Focused");
    ImGui::NextColumn();
    
    ImGui::SetColumnWidth(2, 90);
    if (hovered)
      ImGui::Text("Hovered");
    ImGui::NextColumn();

    ImGui::SetColumnWidth(3, 90);
    ImGui::Text("%d x %d", width, height);
    ImGui::NextColumn();

    ImGui::SetColumnWidth(4, 100);
    auto color = framebuffer->GetSpecification().color;
    if (ImGui::ColorEdit4("", &color.x, ImGuiColorEditFlags_NoInputs)) {
      framebuffer->UpdateSpecificationColor(color);
    }
    ImGui::Columns(1);

    ImGui::PopID();
    ImGui::End();
  }
}

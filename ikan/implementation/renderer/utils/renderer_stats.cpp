//
//  renderer_stats.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "renderer_stats.hpp"
#include "editor/property_grid.hpp"

namespace ikan {
  
  void RendererStatistics::ResetEachFrame() {
    draw_calls = 0;
    index_count = 0;
    vertex_count = 0;
  }
  
  void RendererStatistics::ResetAll() {
    vertex_buffer_size = 0;
    index_buffer_size = 0;
    texture_buffer_size = 0;
    
    draw_calls = 0;
    index_count = 0;
    vertex_count = 0;
  }
  
  RendererStatistics& RendererStatistics::Get() {
    static RendererStatistics instance;
    return instance;
  }
  
  void RendererStatistics::RenderGui() {
    static std::string hovered_message = "";
    
    ImGui::Begin("Renderer Stats");
    ImGui::PushID("Renderer Stats");
    ImGui::BeginChild("##ScrollingRegion", ImVec2(0, ImGui::GetFontSize() * 9), true);

    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, 150);
    ImGui::Text("Vertex Counts");
    ImGui::Text("Index Counts");
    ImGui::Text("Draw Calls");
    ImGui::Text("Vertex Buffer Size");
    ImGui::Text("Index Buffer Size");
    ImGui::Text("Texture Buffer Size");

    ImGui::NextColumn();
    ImGui::Text("%d", vertex_count);
    ImGui::Text("%d", index_count);
    ImGui::Text("%d", draw_calls);
    ImGui::Text("%.2f MB", (float)vertex_buffer_size / (float)1000000.0f);
    hovered_message  = "KB    : " + std::to_string((float)vertex_buffer_size / (float)1000.0f) + "\n";
    hovered_message += "Bytes : " + std::to_string(vertex_buffer_size);
    PropertyGrid::HoveredMsg(hovered_message.c_str());
    
    ImGui::Text("%.2f MB", (float)index_buffer_size / (float)1000000.0f);
    hovered_message  = "KB    : " + std::to_string((float)index_buffer_size / (float)1000.0f) + "\n";
    hovered_message += "Bytes : " + std::to_string(index_buffer_size);
    PropertyGrid::HoveredMsg(hovered_message.c_str());

    ImGui::Text("%.2f MB", (float)texture_buffer_size/ (float)1000000.0f);
    hovered_message  = "KB    : " + std::to_string((float)texture_buffer_size / (float)1000.0f) + "\n";
    hovered_message += "Bytes : " + std::to_string(texture_buffer_size);
    PropertyGrid::HoveredMsg(hovered_message.c_str());

    ImGui::Columns(1);
    
    ImGui::EndChild();
    ImGui::PopID();
    ImGui::End();
  }
  
}

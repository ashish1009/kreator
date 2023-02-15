//
//  renderer_stats.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "renderer_stats.hpp"
#include "editor/property_grid.hpp"

namespace ikan {
  
  void Renderer2DStats::Reset() {
    max_lines = 0;
    max_quads = 0;
    max_circles = 0;
    circles = 0;
    quads = 0;
    lines = 0;
  }
  
  void Renderer2DStats::ResetEachFrame() {
    circles = 0;
    quads = 0;
    lines = 0;
  }
  
  void Renderer2DStats::RenderGui(bool *is_open) {
    if (is_open and *is_open == false) return;
        
    ImGui::Begin("Renderer 2D Stats", is_open);
    ImGui::PushID("Renderer 2D Stats");
    
    ImGui::Columns(6);
    
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%d", max_quads);
    PropertyGrid::HoveredMsg("Max Quads");
    ImGui::NextColumn();
    
    ImGui::SetColumnWidth(1, 80);
    ImGui::Text("%d", quads);
    PropertyGrid::HoveredMsg("Num Quads Rendered");
    ImGui::NextColumn();

    ImGui::SetColumnWidth(2, 80);
    ImGui::Text("%d", max_circles);
    PropertyGrid::HoveredMsg("Max Circles");
    ImGui::NextColumn();
    
    ImGui::SetColumnWidth(3, 80);
    ImGui::Text("%d", circles);
    PropertyGrid::HoveredMsg("Num Circles Rendered");
    ImGui::NextColumn();

    ImGui::SetColumnWidth(4, 80);
    ImGui::Text("%d", max_lines);
    PropertyGrid::HoveredMsg("Max Lines");
    ImGui::NextColumn();
    
    ImGui::SetColumnWidth(5, 80);
    ImGui::Text("%d", lines);
    PropertyGrid::HoveredMsg("Num Lines Rendered");
    ImGui::NextColumn();

    
    ImGui::NextColumn();
    ImGui::Columns(1);

    ImGui::PopID();
    ImGui::End();
  }
  
  Renderer2DStats& Renderer2DStats::Get() {
    static Renderer2DStats instance;
    return instance;
  }

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
  
  void RendererStatistics::RenderGui(bool *is_open, bool summary) {
    if (is_open and *is_open == false) return;

    static std::string hovered_message = "";
    
    ImGui::Begin("Renderer Stats", is_open);
    ImGui::PushID("Renderer Stats");
    
    if (summary) {
      ImGui::Columns(6);
      
      ImGui::SetColumnWidth(0, 80);
      ImGui::Text("%d", draw_calls);
      PropertyGrid::HoveredMsg("Draw Calls");
      ImGui::NextColumn();
      
      ImGui::SetColumnWidth(1, 90);
      ImGui::Text("%d", vertex_count);
      PropertyGrid::HoveredMsg("Vertex Counts");
      ImGui::NextColumn();
      
      ImGui::SetColumnWidth(2, 90);
      ImGui::Text("%d", index_count);
      PropertyGrid::HoveredMsg("Index Counts");
      ImGui::NextColumn();
      
      ImGui::SetColumnWidth(3, 200);
      ImGui::Text("%.3f KB", (float)vertex_buffer_size / (float)1000.0f);
      hovered_message  = "Vertex Buffer Size (Not counting buffer used to render Text) \n";
      hovered_message += "MB    : " + std::to_string((float)vertex_buffer_size / (float)1000000.0f) + "\n";
      hovered_message += "Bytes : " + std::to_string(vertex_buffer_size);
      PropertyGrid::HoveredMsg(hovered_message.c_str());
      ImGui::NextColumn();
      
      ImGui::SetColumnWidth(4, 200);
      ImGui::Text("%.3f KB", (float)index_buffer_size / (float)1000.0f);
      hovered_message  = "Index Buffer Size \n";
      hovered_message += "MB    : " + std::to_string((float)index_buffer_size / (float)1000000.0f) + "\n";
      hovered_message += "Bytes : " + std::to_string(index_buffer_size);
      PropertyGrid::HoveredMsg(hovered_message.c_str());
      ImGui::NextColumn();
      
      ImGui::SetColumnWidth(5, 200);
      ImGui::Text("%.3f KB", (float)texture_buffer_size / (float)1000.0f);
      hovered_message  = "Texure Data Size \n";
      hovered_message += "MB    : " + std::to_string((float)texture_buffer_size / (float)1000000.0f) + "\n";
      hovered_message += "Bytes : " + std::to_string(texture_buffer_size);
      PropertyGrid::HoveredMsg(hovered_message.c_str());

      ImGui::NextColumn();
      ImGui::Columns(1);
    } else {
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
      ImGui::Text("%.3f KB", (float)vertex_buffer_size / (float)1000.0f);
      hovered_message  = "MB    : " + std::to_string((float)vertex_buffer_size / (float)1000000.0f) + "\n";
      hovered_message += "Bytes : " + std::to_string(vertex_buffer_size);
      PropertyGrid::HoveredMsg(hovered_message.c_str());
      
      ImGui::Text("%.3f KB", (float)index_buffer_size / (float)1000.0f);
      hovered_message  = "MB    : " + std::to_string((float)index_buffer_size / (float)1000000.0f) + "\n";
      hovered_message += "Bytes : " + std::to_string(index_buffer_size);
      PropertyGrid::HoveredMsg(hovered_message.c_str());
      
      ImGui::Text("%.3f KB", (float)texture_buffer_size/ (float)1000.0f);
      hovered_message  = "MB    : " + std::to_string((float)texture_buffer_size / (float)1000000.0f) + "\n";
      hovered_message += "Bytes : " + std::to_string(texture_buffer_size);
      PropertyGrid::HoveredMsg(hovered_message.c_str());
      
      ImGui::Columns(1);
      
      ImGui::EndChild();
    }
    ImGui::PopID();
    ImGui::End();
  }
  
}

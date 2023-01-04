//
//  content_browser_panel.cpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#include "content_browser_panel.hpp"

namespace ikan {
  
  ContentBrowserPanel::ContentBrowserPanel(const std::string& root_path)
  : root_path_(root_path), current_directory_(root_path) {
    IK_CORE_TRACE(LogModule::ContentBrowserPannel, "Creating Content Browser Panel ... ");
    IK_CORE_TRACE(LogModule::ContentBrowserPannel, "  Root Path : {0}", root_path_.string());
  }
  
  ContentBrowserPanel::~ContentBrowserPanel() {
    IK_CORE_WARN(LogModule::ContentBrowserPannel, "Destroying Content Browser Panel !!! ");
    IK_CORE_WARN(LogModule::ContentBrowserPannel, "  Root Path : {0}", root_path_.string());
  }

  void ContentBrowserPanel::RenderGui() {
    ImGui::Begin("Content Browser");
    ImGui::PushID("Content Browser");
    
    
    ImGui::PopID(); // Content Browser
    ImGui::End(); // Content Browser
  }
}

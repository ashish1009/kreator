//
//  content_browser_panel.cpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#include "content_browser_panel.hpp"
#include "editor/property_grid.hpp"
#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  ContentBrowserPanel::ContentBrowserPanel(const std::string& root_path)
  : root_path_(root_path), current_directory_(root_path) {
    IK_CORE_TRACE(LogModule::ContentBrowserPanel, "Creating Content Browser Panel ... ");
    IK_CORE_TRACE(LogModule::ContentBrowserPanel, "  Root Path : {0}", root_path_.string());
  }
  
  ContentBrowserPanel::~ContentBrowserPanel() {
    IK_CORE_WARN(LogModule::ContentBrowserPanel, "Destroying Content Browser Panel !!! ");
    IK_CORE_WARN(LogModule::ContentBrowserPanel, "  Root Path : {0}", root_path_.string());
  }

  void ContentBrowserPanel::RenderGui() {
    ImGui::Begin("Content Browser");
    ImGui::PushID("Content Browser");
    
    TitleIcon();
    
    // Side Menue should be called before MainArea
    SideMenu();
    ImGui::SameLine();
    
    MainArea();
    
    ImGui::PopID(); // Content Browser
    ImGui::End(); // Content Browser
  }
  
  void ContentBrowserPanel::TitleIcon() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("Title Area",
                      ImVec2(ImGui::GetWindowContentRegionWidth(), 35.0f),
                      true, // Border
                      ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Columns(3);
    ImGui::SetColumnWidth(0, 100);
    
    Back();             ImGui::SameLine();
    Forward();          ImGui::SameLine();
    Home();             ImGui::SameLine();
    
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, 200);
    
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, (35.0f - 20.0f) / 2));
    Search();
    
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, 200);
    
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, (35.0f - 20.0f) / 2));
    PathHistory();
    ImGui::NextColumn();
    ImGui::Columns(1);
    
    ImGui::EndChild();
    ImGui::PopStyleVar();
  }
  
  void ContentBrowserPanel::SideMenu() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RoundFactor);
    ImGui::BeginChild("SideArea",
                      ImVec2(ImGui::GetWindowContentRegionWidth() * SideChildWidth,
                             ImGui::GetWindowHeight() - WindowYOffset),
                      true, // Border
                      ImGuiWindowFlags_HorizontalScrollbar);
    
    ImGui::EndChild();
    ImGui::PopStyleVar();
  }
  
  void ContentBrowserPanel::MainArea() {
    // Push style
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RoundFactor);
    ImGui::BeginChild("MainArea",
                      ImVec2(ImGui::GetWindowContentRegionWidth() * (0.995f - SideChildWidth),
                             ImGui::GetWindowHeight() - WindowYOffset),
                      true, // Border
                      ImGuiWindowFlags_HorizontalScrollbar);
    
    ImGui::EndChild();
    ImGui::PopStyleVar();
  }
  
  void ContentBrowserPanel::Back() {
    static std::shared_ptr<Texture> back_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/back.png"));
    if (PropertyGrid::ImageButton("Back",
                                  back_texture->GetRendererID(),
                                  { 18.0f, 18.0f })) {
    }
    PropertyGrid::HoveredMsg("NOTE: Feature not implemented", true);
  }
  
  void ContentBrowserPanel::Forward() {
    static std::shared_ptr<Texture> forward_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/forward.png"));
    if (PropertyGrid::ImageButton("Forward",
                                  forward_texture->GetRendererID(),
                                  { 18.0f, 18.0f })) {
    }
    PropertyGrid::HoveredMsg("NOTE: Feature not implemented", true);
  }
  
  void ContentBrowserPanel::Home() {
    static std::shared_ptr<Texture> home_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/home.png"));
    if (PropertyGrid::ImageButton("home",
                                  home_texture->GetRendererID(),
                                  { 18.0f, 18.0f })) {
      // Change the current directory
      current_directory_ = root_path_;
    }
  }
  
  void ContentBrowserPanel::Search() {
    // Search filter
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 16);
    if (PropertyGrid::Search(search_filter_.InputBuf,
                             "Search ... ",
                             { 18.0f, 18.0f })) {
      search_filter_.Build();
    }
  }
  
  void ContentBrowserPanel::PathHistory() {
  }

}

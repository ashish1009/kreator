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
  
  ContentBrowserPanel::ContentBrowserPanel(const std::string& root_path,
                                           const std::vector<std::filesystem::path>& favourite_paths)
  : root_path_(root_path), current_directory_(root_path) {
    // Add Pinned Paths
    favourite_paths_.emplace_back("../../../ikan/core_assets");
    for (const auto& path : favourite_paths)
      favourite_paths_.emplace_back(path);

    IK_CORE_TRACE(LogModule::ContentBrowserPanel, "Creating Content Browser Panel ... ");
    IK_CORE_TRACE(LogModule::ContentBrowserPanel, "  Root Path : {0}", root_path_.string());
  }
  
  ContentBrowserPanel::~ContentBrowserPanel() {
    IK_CORE_WARN(LogModule::ContentBrowserPanel, "Destroying Content Browser Panel !!! ");
    IK_CORE_WARN(LogModule::ContentBrowserPanel, "  Root Path : {0}", root_path_.string());
  }
  
  void ContentBrowserPanel::SetRootData(const std::string& root_path) {
    root_path_ = root_path;
    current_directory_ = root_path;
  }

  void ContentBrowserPanel::AddFavouritPaths(const std::vector<std::filesystem::path>& favourite_paths) {
    for (const auto& path : favourite_paths)
      favourite_paths_.emplace_back(path);
  }

  void ContentBrowserPanel::RenderGui(bool* is_open) {
    if (is_open and *is_open == false) return;
    
    ImGui::Begin("Content Browser", is_open);
    ImGui::PushID("Content Browser");
    
    side_child_width_ = ImGui::GetContentRegionAvailWidth() * 0.20;
    
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
                      ImVec2(side_child_width_,
                             ImGui::GetWindowHeight() - WindowYOffset),
                      true, // Border
                      ImGuiWindowFlags_HorizontalScrollbar);
    
    // Flag for the child menu
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_SpanAvailWidth |
    ImGuiTreeNodeFlags_Framed |
    ImGuiTreeNodeFlags_DefaultOpen;
    
    // Shows the content of the side pinned folders
    bool menu_opened = ImGui::TreeNodeEx((void*)"##SizeMenu", flags, "Favourits");
    if (menu_opened) {
      // TODO: Add feature to add pinned path in run time
      int32_t push_id = 0;
      for (auto pinned_path : favourite_paths_) {
        ImGui::PushID(++push_id);
        
        // Get the filename of the side menu content
        const auto& filename = pinned_path.filename().c_str();
        auto flag = ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_SpanAvailWidth
        | ((pinned_path == current_directory_) ? ImGuiTreeNodeFlags_Selected : 0);
        
        // Show the Pinned filder name
        bool opened = ImGui::TreeNodeEx(filename,
                                        flag,
                                        filename);
        
        // If click then jump to that directosy
        if (ImGui::IsItemClicked()) {
          //TODO: For now clear everything
          path_hierarchy_.clear();

          // Change current directory
          current_directory_ = pinned_path;
          path_hierarchy_.emplace_back(current_directory_);
        }

        // If opened the menu of current folder then show its content
        if (opened) {
          // TODO: Add Function
          ImGui::TreePop();
        }
        
        ImGui::PopID(); // ImGui::PushID(push_id);
      } // for (auto pinned_path : pinned_paths_)
      ImGui::TreePop();
    } // if (menu_opened)

    ImGui::EndChild();
    ImGui::PopStyleVar();
  }
  
  void ContentBrowserPanel::MainArea() {
    static std::shared_ptr<Texture> folder_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/folder.png"));
    static std::shared_ptr<Texture> file_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/file.png"));
    static std::shared_ptr<Texture> jpg_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/jpg.png"));
    static std::shared_ptr<Texture> png_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/png.png"));
    
    static std::shared_ptr<Texture> obj_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/obj.png"));
    static std::shared_ptr<Texture> fbx_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/fbx.png"));
    
    static std::shared_ptr<Texture> font_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/font.png"));
    
    static std::shared_ptr<Texture> cpp_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/cpp.png"));
    static std::shared_ptr<Texture> c_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/c.png"));
    static std::shared_ptr<Texture> h_texture = Renderer::GetTexture(AM::CoreAsset("textures/content_browser/h.png"));

    // Push style
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RoundFactor);
    ImGui::BeginChild("MainArea",
                      ImVec2(ImGui::GetWindowContentRegionWidth() - side_child_width_,
                             ImGui::GetWindowHeight() - WindowYOffset),
                      true, // Border
                      ImGuiWindowFlags_HorizontalScrollbar);
    
    // Get the current cursor position of imgui widget
    static ImVec2 init_pos = ImGui::GetCursorPos();
    
    int32_t push_id = 0; // id for each button
    // Travers entire directory
    for (const auto& directory : std::filesystem::directory_iterator(current_directory_)) {
      // Get the file name from the directory
      const auto& path = directory.path();
      const std::string& filename_string = path.filename().string();

      // If search result passes
      if (search_filter_.PassFilter(filename_string.c_str())) {
        std::shared_ptr<Texture> icon_texture;
        static bool is_directory = false;

        // Set the icon if current file is a direcotry/folder else use specific file texture
        if (directory.is_directory()) {
          icon_texture = folder_texture;
          is_directory = true;
        } else {
          if (".png" == path.extension()) icon_texture = png_texture;
          else if (".jpg" == path.extension()) icon_texture = jpg_texture;
          else if (".cpp" == path.extension()) icon_texture = cpp_texture;
          else if (".h" == path.extension()) icon_texture = h_texture;
          else if (".c" == path.extension()) icon_texture = c_texture;
          else if (".obj" == path.extension()) icon_texture = obj_texture;
          else if (".fbx" == path.extension()) icon_texture = fbx_texture;
          else if (".ttf" == path.extension()) icon_texture = font_texture;
          else icon_texture = file_texture;
          
          is_directory = false;
        }
        
        // Icon Button size
        static float icon_size_height = 64.0f;
        static float icon_size_width  = 50.0f;

        ImGui::PushID(filename_string.c_str());

        // Update the cursor for each folder/file based on its position
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((icon_size_width + 30.0f) * push_id), init_pos.y));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

        // Render the image button for each folder/file
        bool pressed = PropertyGrid::ImageButton(push_id,
                                                 icon_texture->GetRendererID(),
                                                 { icon_size_width, icon_size_height });

        // If icon is clicked Do some action
        if (pressed) {
          if (is_directory) {
            // Store the current directory in path history
            back_path_history_.emplace_back(current_directory_);
            
            // Clear the forward path history as we are at the top already
            while (!forward_path_history_.empty()) {
              forward_path_history_.pop_back();
            }

            // Change the current directory
            current_directory_ /= path.filename();

            // Store the current path in path hierarchy
            path_hierarchy_.emplace_back(current_directory_);
          } else {
            // DO NOTHING
          }
        } // Pressed?
          // Drag the content from here
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
          const std::string& dragged_path = directory.path().string();
          const char* ch = dragged_path.c_str();
          ImGui::SetDragDropPayload("SelectedFile",
                                    (void*)ch,
                                    dragged_path.size(),
                                    ImGuiCond_Always);
          
          PropertyGrid::ImageButton(push_id,
                                    icon_texture->GetRendererID(),
                                    { 20.0f, 20.0f });
          ImGui::EndDragDropSource();
        }
        ImGui::PopID(); // ImGui::PushID(filename_string.c_str());

        // Text Button : Directory name
        static float wrapWidth = 70.0f;
        ImGui::PushID("CBP_Main_Area_FileNameButton");
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((icon_size_width + 30.0f) * push_id),
                                   init_pos.y + icon_size_height + 10.0f));
        
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);
        ImGui::Button(filename_string.c_str(), ImVec2(icon_size_width + 5, 20));
        PropertyGrid::HoveredMsg(filename_string.c_str());
        ImGui::PopTextWrapPos();

        ImGui::PopStyleColor(); // ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PopID(); // ImGui::PushID("CBP_Main_Area_FileNameButton");

        push_id++;
      } // Pass Filter
    } // Directory Iterator

    ImGui::EndChild();
    ImGui::PopStyleVar();
  }
  
  void ContentBrowserPanel::Back() {
    static std::shared_ptr<Texture> back_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/back.png"));
    if (PropertyGrid::ImageButton("Back",
                                  back_texture->GetRendererID(),
                                  { 18.0f, 18.0f })) {
      // Do nothing if path history is empty
      if (back_path_history_.empty())
        return;
      
      // Remove the element from path hierarchy
      path_hierarchy_.pop_back();

      // Store the current path in forward history
      forward_path_history_.emplace_back(current_directory_);

      // Change the current directory and remove element from path history
      current_directory_ = back_path_history_.back();
      back_path_history_.pop_back();
    }
  }
  
  void ContentBrowserPanel::Forward() {
    static std::shared_ptr<Texture> forward_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/forward.png"));
    if (PropertyGrid::ImageButton("Forward",
                                  forward_texture->GetRendererID(),
                                  { 18.0f, 18.0f })) {
      // Do nothing if path history is empty
      if (forward_path_history_.empty())
        return;

      // Store the current path in back history
      back_path_history_.emplace_back(current_directory_);

      // Change the current directory and remove element from path history
      current_directory_ = forward_path_history_.back();
      forward_path_history_.pop_back();

      // Store the current path in path hierarchy
      path_hierarchy_.emplace_back(current_directory_);
    }
  }
  
  void ContentBrowserPanel::Home() {
    static std::shared_ptr<Texture> home_texture = Renderer::GetTexture(AM::CoreAsset("textures/icons/home.png"));
    if (PropertyGrid::ImageButton("home",
                                  home_texture->GetRendererID(),
                                  { 18.0f, 18.0f })) {
      // Do nothing if current path is same as root
      if (current_directory_ == std::filesystem::path(root_path_)) {
        return;
      }
      
      // Change the current directory
      current_directory_ = root_path_;
      
      // Clear the forward path history as we are at the top already
      forward_path_history_.clear();
      
      // Clear the back path history as we are not considering any back from
      // home for now
      back_path_history_.clear();
      
      // Clear the path hierarchy and add only home path
      path_hierarchy_.clear();
      path_hierarchy_.emplace_back(current_directory_);
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
    size_t i = 0;
    for (const auto& path : path_hierarchy_) {
      i++;
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
      if (ImGui::Button(path.filename().c_str())) {
      }
      ImGui::PopStyleColor();
      ImGui::SameLine();
      
      if (i != path_hierarchy_.size())
        ImGui::Text(">");
      ImGui::SameLine();
    }
  }

  const std::filesystem::path& ContentBrowserPanel::GetCurrentDir() const {
    return current_directory_;
  }
  
  const std::filesystem::path& ContentBrowserPanel::GetRootDir() const {
    return root_path_;
  }

}

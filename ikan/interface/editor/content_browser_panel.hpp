//
//  content_browser_panel.hpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#pragma once

// This class includes the Content browser panel class

namespace ikan {
  
  /// This class renders imgui window to access contents of folders
  class ContentBrowserPanel {
  public:
    /// This constructor creates content Browser Panel instance with root path
    /// - Parameter root_path: root path of Content browser panel
    ContentBrowserPanel(const std::string& root_path = "../../../",
                        const std::vector<std::filesystem::path>& favourite_paths = {});
    /// This destructor destroy Content browser panel instance
    ~ContentBrowserPanel();
    
    /// This function renders Imgui for Contenct browser panel
    void RenderGui();
    /// This funcfion add the favourit paths to the pannel
    /// - Parameter favourite_paths: paths
    void AddFavouritPaths(const std::vector<std::filesystem::path>& favourite_paths);

    DELETE_COPY_MOVE_CONSTRUCTORS(ContentBrowserPanel)
    
  private:
    // -------------------
    // Member Functions
    // -------------------
    /// This function renders the title of Content browser pannel
    void TitleIcon();
    /// This function renders the Main area of Content browser pannel
    void MainArea();
    /// This function renders the Main area of Content browser pannel
    void SideMenu();
    
    /// This function render back icon
    void Back();
    /// This function render forward icon
    void Forward();
    /// This function render home icon
    void Home();
    
    /// This function render search Text box icon
    void Search();
    /// This fucntion prints the path history
    void PathHistory();

    // -------------------
    // Member Functions
    // -------------------
    static constexpr float SideChildWidth = 0.15f;
    static constexpr float WindowYOffset  = 55.0f;
    static constexpr float RoundFactor    = 2.0f;
    
    std::filesystem::path root_path_, current_directory_;
    ImGuiTextFilter search_filter_;

    std::vector<std::filesystem::path> favourite_paths_;
  };
  
}

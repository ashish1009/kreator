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
    ContentBrowserPanel(const std::string& root_path = "../../../");
    /// This destructor destroy Content browser panel instance
    ~ContentBrowserPanel();
    
    /// This function renders Imgui for Contenct browser panel
    void RenderGui();

    DELETE_COPY_MOVE_CONSTRUCTORS(ContentBrowserPanel)
    
  private:
    std::filesystem::path root_path_, current_directory_;
  };
  
}

//
//  text_renderer.hpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

// Text Renderer APIs
// Responsible for
// - Initialize the Text rednder
// - User friendly API to renderant string

namespace ikan {
  
  /// This class is the wrapper for storing Text Renderer API
  class TextRenderer {
  public:
    /// This function initializes the Text renderer. Create All buffers needed to store Data (Both Renderer and CPU)
    static void Init();
    /// This function Shuts down on Text the Batch Renderer. Delete all the allocated Data
    static void Shutdown();
    
    /// This function loads the font for freetype
    /// - Parameter font_file_path: path of font
    /// NOTE: Load the Font before using any other API In clientw
    static void LoadFreetype(const std::string& font_file_path);
  };
  
}

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
    
    /// This function begins the Batch for 2D Rendere (to be called each frame)
    /// - Parameters:
    ///   - camera_view_projection_matrix: Camera View projection Matrix
    static void BeginBatch(const glm::mat4& camera_view_projection_matrix);
    /// This function Ends the current batch by rendering all the vertex
    static void EndBatch();

    /// This functions renders the Text in Window using postition and sclae
    /// - Parameters:
    ///   - text: Text to be rendereed
    ///   - position: Text Poistion
    ///   - scale: Text Poistion
    ///   - color: Color of text
    static void RenderFixedViewText(std::string text, glm::vec3 position, const glm::vec2& scale, const glm::vec4& color);
    /// This functions renders the Text in Window using postition and sclae
    /// - Parameters:
    ///   - text: Text to be rendereed
    ///   - position: Text Poistion
    ///   - scale: Text Poistion
    ///   - color: Color of text
    static void RenderText(std::string text, glm::vec3 position, const glm::vec2& scale, const glm::vec4& color);

    MAKE_PURE_STATIC(TextRenderer);
    
  private:
    /// This functions renders the Text in Window using postition and sclae
    /// - Parameters:
    ///   - text: Text to be rendereed
    ///   - position: Text Poistion
    ///   - scale: Text Poistion
    ///   - color: Color of text
    static void RenderTextImpl(std::string text, glm::vec3 position, const glm::vec2& scale, const glm::vec4& color);

    /// This function flsh a single batch
    static void Flush();
    /// This function moves to next batch in single frame
    static void NextBatch();
  };
  
}

//
//  open_gl_renderer_id_manager.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "renderer/utils/renderer.hpp"

namespace ikan {
  
  /// This class Manages all the renderer IDs. If nedded add set to store unique ID in their respective
  /// '_GetId' functions
  class OpenGlRendererIdManager {
  public:
    /// This function updates the Open GL Renderer ID for Shader
    /// - Parameter renderer_id: renderer id reference
    static void GetShaderId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Buffers
    /// - Parameter renderer_id: renderer id reference
    static void GetBufferId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Pipeline
    /// - Parameter renderer_id: renderer id reference
    static void GetPipelineId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Texture
    /// - Parameter renderer_id: renderer id reference
    static void GetTextureId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Frame buffer
    /// - Parameter renderer_id: renderer id reference
    static void GetFramebufferId(RendererID &renderer_id);
    
    /// This function updates the Open GL Renderer ID for Shader
    /// - Parameter renderer_id: renderer id reference
    static void RemoveShaderId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Buffers
    /// - Parameter renderer_id: renderer id reference
    static void RemoveBufferId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Pipeline
    /// - Parameter renderer_id: renderer id reference
    static void RemovePipelineId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Texture
    /// - Parameter renderer_id: renderer id reference
    static void RemoveTextureId(RendererID &renderer_id);
    /// This function updates the Open GL Renderer ID for Frame buffer
    /// - Parameter renderer_id: renderer id reference
    static void RemoveFramebufferId(RendererID &renderer_id);

    
    MAKE_PURE_STATIC(OpenGlRendererIdManager);
  };
  
  using IDManager = OpenGlRendererIdManager;
}

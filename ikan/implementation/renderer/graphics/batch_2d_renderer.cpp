//
//  batch_2d_renderer.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "batch_2d_renderer.hpp"
#include "renderer/utils/renderer_stats.hpp"
#include "renderer/graphics/pipeline.hpp"
#include "renderer/graphics/renderer_buffer.hpp"
#include "renderer/graphics/shader.hpp"
#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  /// Maximum Texture slot supported by Senderer Shader Current Open GL Version 4.1 supports
  /// only 16 Texture slot in Shader
  /// TODO: Change this value based on the Open GL Version
  static constexpr uint32_t kMaxTextureSlotsInShader = 16;
  
  static constexpr glm::vec2 texture_coords_[] = {
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
  };
  
  struct BaseVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texture_coords;
    
    float texture_index;
    float tiling_factor;
  };

  /// This structure holds the common batch renderer data for Quads and circle
  struct BatchRendererData {
    /// This structure stores the environmant data
    struct Environment {
      glm::mat4 camera_view_projection_matrix;
      glm::mat4 camera_view_matrix;
    };
    
    /// Store the Vertex and Indices size
    uint32_t max_vertices = 0;
    uint32_t max_indices = 0;
    
    /// Store the Environment data
    Environment environment;
    
    /// Renderer Data storage
    std::shared_ptr<Pipeline> pipeline;
    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::shared_ptr<Shader> shader;
    
    /// Count of Indices to be renderer in Single Batch
    uint32_t index_count = 0;
    
    /// Stores all the 16 Texture in array so that there is no need to load texture each frame
    /// NOTE: Load only if new texture is added or older replaced with new
    std::array<std::shared_ptr<Texture>, kMaxTextureSlotsInShader> texture_slots;
    
    /// Texture Slot index sent to Shader to render a specific Texture from slots
    /// Slot 0 is reserved for white texture (No Image only color)
    uint32_t texture_slot_index = 1; // 0 = white texture
    
    /// Basic vertex of quad
    /// Vertex of circle is taken as Quad only
    glm::vec4 vertex_base_position[4];
    
    /// Virtual Destructor
    virtual ~BatchRendererData() = default;
  private:
    BatchRendererData() = default;
  };

  // --------------------------------------------------------------------------
  // Batch Renderer API
  // --------------------------------------------------------------------------
  void BatchRenderer::Init() {
    IK_CORE_INFO("Initialising the Batch Renderer 2D ...");
  }

  void BatchRenderer::Shutdown() {
    IK_CORE_WARN("Shutting Down the Batch Renderer 2D !!!");
  }
  
}

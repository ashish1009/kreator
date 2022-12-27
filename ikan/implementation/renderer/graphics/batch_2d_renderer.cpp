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
    // Constants
    static constexpr uint32_t VertexForSingleElement = 4;
    static constexpr uint32_t IndicesForSingleElement = 6;

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

    /// Max element to be rendered in single batch
    uint32_t max_element = 0;

    /// Basic vertex of quad
    /// Vertex of circle is taken as Quad only
    glm::vec4 vertex_base_position[4];
        
    void StartCommonBatch() {
      index_count = 0;
      texture_slot_index = 1;
    }
    
    /// Virtual Destructor
    virtual ~BatchRendererData() {
      RendererStatistics::Get().index_buffer_size -= max_indices * sizeof(uint32_t);
    }

    friend class QuadData;
    friend class CircleData;

  private:
    BatchRendererData() = default;
  };
  
  /// Batch Data to Rendering Quads
  struct QuadData : BatchRendererData {
    /// Single vertex of a Quad
    struct Vertex : BaseVertex {
      int32_t object_id;        // Pixel ID of Quad
    };
    
    /// Base pointer of Vertex Data. This is start of Batch data for single draw call
    Vertex* vertex_buffer_base_ptr = nullptr;
    /// Incrememntal Vetrtex Data Pointer to store all the batch data in Buffer
    Vertex* vertex_buffer_ptr = nullptr;
    
    /// Constructor
    QuadData() {
      IK_CORE_TRACE("Creating QuadData instance ...");
    }
    /// Destructor
    virtual ~QuadData() {
      IK_CORE_WARN("Destroying QuadData instance and clearing the data !!!");
      delete [] vertex_buffer_base_ptr;
      vertex_buffer_base_ptr = nullptr;
      
      RendererStatistics::Get().vertex_buffer_size -= max_vertices * sizeof(QuadData::Vertex);
    }
    
    /// start new batch for quad rendering
    void StartBatch() {
      StartCommonBatch();
      vertex_buffer_ptr = vertex_buffer_base_ptr;
    }
  };
  static QuadData* quad_data_;
  
  /// Batch Data to Rendering Circles
  struct CircleData : BatchRendererData {
    /// Single vertex of a Circle
    struct Vertex : BaseVertex {
      float thickness;    // Thickness of Circle
      float fade;         // Fadeness of Edge of Circle
      
      int32_t object_id; // Pixel ID of Quad
    };
        
    /// Base pointer of Vertex Data. This is start of Batch data for single draw call
    Vertex* vertex_buffer_base_ptr = nullptr;
    /// Incrememntal Vetrtex Data Pointer to store all the batch data in Buffer
    Vertex* vertex_buffer_ptr = nullptr;
    
    /// Constructor
    CircleData() {
      IK_CORE_TRACE("Creating CircleData instance ...");
    }
    /// Destructir
    virtual ~CircleData() {
      IK_CORE_WARN("Destroying Circle Data instance and clearing the data !!!");
      delete [] vertex_buffer_base_ptr;
      vertex_buffer_base_ptr = nullptr;
      
      RendererStatistics::Get().vertex_buffer_size -= CircleData::max_vertices * sizeof(CircleData::Vertex);
    }
    
    /// start new batch for quad rendering
    void StartBatch() {
      StartCommonBatch();
      vertex_buffer_ptr = vertex_buffer_base_ptr;
    }
  };
  static CircleData* circle_data_;
  
  // --------------------------------------------------------------------------
  // Batch Renderer API
  // --------------------------------------------------------------------------
  void BatchRenderer::Init() {
    IK_CORE_INFO("Initialising the Batch Renderer 2D ...");
    InitQuadData(10);
    InitCircleData(10);
  }

  void BatchRenderer::Shutdown() {
    IK_CORE_WARN("Shutting Down the Batch Renderer 2D !!!");
  }
  
  void BatchRenderer::InitQuadData(uint32_t max_quads) {
  }
  
  void BatchRenderer::InitCircleData(uint32_t max_circles) {
  }
  
}

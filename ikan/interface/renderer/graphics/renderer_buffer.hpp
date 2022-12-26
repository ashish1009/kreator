//
//  renderer_buffer.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the interface for creating buffers in GPU

#include "renderer/utils/renderer.hpp"

namespace ikan {
 
  /// This class is the interface of Renderer Vertex Buffer, to store the vertices of the objects.
  class VertexBuffer {
  public:
    // -------------
    // Destrcutor
    // -------------
    virtual ~VertexBuffer() = default;

    // -----------------
    // Static Function
    // -----------------
    /// This static function creates the instance of Vertex Buffer based on the supported API. This is
    /// static data buffer which takes the data pointer in the argument
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    static std::shared_ptr<VertexBuffer> Create(void* data, uint32_t size);
  };

  /// This class is the interface of Renderer Vertex Buffer, to store the vertices of the objects.
  class IndexBuffer {
  public:
    // -------------
    // Destrcutor
    // -------------
    virtual ~IndexBuffer() = default;
    
    // -----------------
    // Static Function
    // -----------------
    /// This static function creates the instance of Index Buffer based on the supported API. This api
    /// takes the data buffer and number of indices
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - count: count of indiced (NOTE: Single indices is taken as uint32_t)
    static std::shared_ptr<IndexBuffer> CreateWithCount(void* data, uint32_t count);
  };

}

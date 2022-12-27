//
//  pipeline.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the interface of Renderer Pipeline. To store the buffers in
// Graphics Processor unit.

#include "renderer/utils/renderer.hpp"
#include "renderer/graphics/renderer_buffer.hpp"

namespace ikan {
  
  /// Interface class for Storing Renderer Pipeline. Implementation is depending on the supported Renerer API.
  class Pipeline {
  public:
    // -------------
    // Destrcutor
    // -------------
    virtual ~Pipeline() noexcept = default;

    // ---------------
    // Fundamentals
    // ---------------
    /// This function add the Vertex Buffer inside Pipeline and set attribute of each vertices in GPU
    /// - Parameter vertexBuffer: Ref type of Vertex Buffer
    virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;

    // -----------------
    // Static Function
    // -----------------
    /// This static function creates Ref type instance of Pipeline. Instance depending on the current Renderer API
    static std::shared_ptr<Pipeline> Create();
  };
  
}

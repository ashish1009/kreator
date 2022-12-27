//
//  pipeline.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the implementaiton of Open GL Pipeline

#include "renderer/graphics/pipeline.hpp"

namespace ikan {
  
  class OpenGLPipeline : public Pipeline {
  public:
    // ---------------------------------
    // Constructors and Destructor
    // ---------------------------------
    /// This constructor create the Vertex Array in Graphics to store the
    /// buffers
    OpenGLPipeline();
    /// This destructor deletes the Vertex Array and its buffers
    ~OpenGLPipeline() noexcept;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLPipeline);
    
    // ---------------
    // Fundamentals
    // ---------------
    /// This function add the Vertex Buffer inside Pipeline and set attribute of each vertices in GPU
    /// - Parameter vertexBuffer: Ref type of Vertex Buffer
    void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
    /// This function updates the current Index Buffer inside the Pipeline
    /// - Parameter indexBuffer Ref type of Index Buffer
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
    /// Bind the current pipeline to the GPU
    void Bind() const override;
    /// Unbind the current pipeline to the GPU
    void Unbind() const override;

    // ---------
    // Getters
    // ---------
    /// This function returns all the Vertex Buffer Stored in Pipeline
    const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override;
    /// This function returns the Current Index Buffer Stored in Pipeline
    const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override;

  private:
    RendererID renderer_id_ = 0;
    std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers_;
    std::shared_ptr<IndexBuffer> index_buffer_;
  };
  
} // namespace ikan

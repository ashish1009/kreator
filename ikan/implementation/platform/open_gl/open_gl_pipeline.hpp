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
    
  private:
    RendererID renderer_id_ = 0;
  };
  
} // namespace ikan

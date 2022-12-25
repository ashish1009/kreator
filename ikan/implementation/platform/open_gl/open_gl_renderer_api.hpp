//
//  open_gl_renderer_api.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the implenentation of Renderer API Class

#include "renderer/utils/renderer_api.hpp"

namespace ikan {
  
  /// This class the implementation of Renderer API
  class OpenGLRendererAPI : public RendererAPI {
  public:
    // -----------------------------------
    // Constructors and Destructor
    // -----------------------------------
    /// This is the default constructor of Rendere API
    OpenGLRendererAPI();
    /// This is the default destructor ot Renderer API
    virtual ~OpenGLRendererAPI() noexcept;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLRendererAPI);
    
    // ----------------
    // Fundamentals
    // ----------------
    /// This function initialize the respective renderer api
    void Init() const override;
    /// This function shutdowns the respectiev renderer api
    void Shutdown() const override;
  };
  
} 

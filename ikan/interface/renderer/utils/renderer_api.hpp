//
//  renderer_api.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the interface to hold all the renderer API whose implementation
// depetnds on the supported API

namespace ikan {
    
  /// This class is the interface for Renderer APIs
  class RendererAPI {
  public:
    /// This is the default destrutctor
    virtual ~RendererAPI() = default;
    
    /// This function initialize the respective renderer api
    virtual void Init() const = 0;
    /// This function shutdowns the respectiev renderer api
    virtual void Shutdown() const = 0;
    
    /// This function creates the instance of renderer API based on the supported API
    static std::unique_ptr<RendererAPI> Create();
  };
  
}

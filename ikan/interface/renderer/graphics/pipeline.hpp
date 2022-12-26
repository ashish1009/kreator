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

namespace ikan {
  
  /// Interface class for Storing Renderer Pipeline. Implementation is depending on the supported Renerer API.
  class Pipeline {
  public:
    // -------------
    // Destrcutor
    // -------------
    virtual ~Pipeline() noexcept = default;

    // -----------------
    // Static Function
    // -----------------
    /// This static function creates Ref type instance of Pipeline. Instance depending on the current Renderer API
    static std::shared_ptr<Pipeline> Create();
  };
  
}

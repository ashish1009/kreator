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
    
    /// This function changes the bit value for depth bit
    /// - Parameter state: new depth state
    void Depth(bool state) const override;
    /// This functin changes the bit value for Blend bit
    /// - Parameter state: new Blend state
    void Blend(bool state) const override;
    /// This function changes the bit value of multisample
    /// - Parameter state: new Multi sample state
    void MultiSample(bool state) const override;
    /// This function change depth function
    /// - Parameter func: depth function type
    void DepthFunc(GlFunc func) const override;
    
    /// This function begins the wireframe renderer
    void BeginWireframe() const override;
    /// This function ends the wireframe renderer
    void EndWireframe() const override;
    
    /// This function clear all the renderer bits and set the clear color as 'color'
    /// - Parameter color: new clear color of screen
    void SetClearColor(const glm::vec4& color) const override;
    /// This function clear all the bits of renderer screen
    void ClearBits() const override;
    /// This function Clears Depth Bit
    void ClearDepthBit() const override;
    /// This function Clears Color Biut
    void ClearColorBit() const override;
    /// This function Clears Stensil Bit
    void ClearStencilBit() const override;
    
    /// This function update the renderer viewport size
    /// - Parameters:
    ///   - width: new width of renderer viewport
    ///   - height: new height of renderer viewport
    void SetViewport(uint32_t width, uint32_t height) const override;
    
    /// This funciton returns the Pixel ID from Frame buffer
    /// - Parameters:
    ///   - mx: x pixel
    ///   - my: y pixel
    ///   - pixel_data: pixel value
    ///   - pixel_id_index: pixel index
    void GetEntityIdFromPixels(int32_t mx,
                               int32_t my,
                               uint32_t pixel_id_index,
                               int32_t& pixelData) const override;
    
    // -------------
    // Draw API
    // -------------
    /// This API draws a quad with pipeline and indexed count
    /// - Parameters:
    ///   - pipeline: pipeline having vertex buffer and index buffer
    ///   - count: number of Indices (if 0 then use index buffer of Vertex array)
    void DrawIndexed(const std::shared_ptr<Pipeline>& pipeline,
                     uint32_t count) const override;

  };
  
} 

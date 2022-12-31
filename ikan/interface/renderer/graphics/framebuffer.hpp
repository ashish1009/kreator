//
//  framebuffer.hpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

// This file includes the interface of renderer framebuffer.

#include "renderer/utils/renderer.hpp"

namespace ikan {
  
  /// This class is the interface for the renderer frame buffer
  class FrameBuffer {
  public:
    /// This structure stores the Attachments of Frame buffer. NOTE: Separate class to use initializer
    /// list in constructor of FrameBuffer::Specification
    struct Attachments {
      /// Texture format type
      enum class TextureFormat {
        None = 0,
        RGBA8,
        Depth24Stencil
      };
      
      Attachments(std::initializer_list<TextureFormat> attachments);

      // default constructor and destructors
      Attachments() = default;
      ~Attachments() = default;
      
      // Copy and Move Constructor and operator =
      DEFINE_COPY_MOVE_CONSTRUCTORS(Attachments);
      
      // Member veriable
      std::vector<TextureFormat> texture_formats;
    };

    /// This structure stores the Framebuffer specification
    struct Specification {
      uint32_t width = 2100, height = 900;
      glm::vec4 color = { 0.1f, 0.1f, 0.1f, 1.0f };
      Attachments attachments;

      Specification();
      ~Specification();
      // Copy and Move Constructor and operator =
      DEFINE_COPY_MOVE_CONSTRUCTORS(Specification);
    };
    
    // -------------
    // Destrcutor
    // -------------
    /// This is the defult destructor of interface FrameBuffer
    virtual ~FrameBuffer() = default;
    
    // -------------
    // Fundamentals
    // -------------
    /// This function binds the current FrameBuffer to the renderer
    virtual void Bind() const = 0;
    /// This function unbinds the current FrameBuffer from the renderer
    virtual void Unbind() const = 0;
    /// This function resizes the Framebuffer
    /// - Parameters:
    ///   - width: New width of FB
    ///   - height: New height of FB
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    
    /// This funciton updates the specification color
    /// - Parameter color: color
    virtual void UpdateSpecificationColor(const glm::vec4& color) = 0;
    
    // -------------
    // Getters
    // -------------
    /// This function returns the Frame buffer specification
    [[nodiscard]] virtual const Specification& GetSpecification() const = 0;
    /// This function returns the Renderer ID
    [[nodiscard]] virtual RendererID GetRendererId() const = 0;
    /// This function returns the Depth Attachment ID
    [[nodiscard]] virtual RendererID GetDepthAttachmentId() const = 0;
    /// This function returns the Color Attachment iDs
    [[nodiscard]] virtual const std::vector<RendererID>& GetColorAttachmentIds() const = 0;

    // -----------------
    // Static Function
    // -----------------
    /// This static function creates the Framebuffer instance based on the current Supported API
    /// - Parameter spec: Frame buffer specification
    [[nodiscard]] static std::shared_ptr<FrameBuffer> Create(const Specification& spec = Specification());
  };
  
}

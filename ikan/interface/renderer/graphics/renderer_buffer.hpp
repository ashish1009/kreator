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
  
  /// This enum shoes the data type to be stored in shader
  enum class ShaderDataType : uint8_t {
    NoType = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
  };
  
  /// This structur stores all the property of a element in a buffer.
  struct BufferElement {
    std::string name = "";
    ShaderDataType type = ShaderDataType::NoType;
    uint32_t size = 0;
    uint32_t count = 0;
    size_t offset = 0;
    bool normalized = false;
    
    // -------------------
    // Constructor
    // ------------------
    /// This Constructor creates the buffer element with arguments
    /// - Parameters:
    ///   - name: name of element
    ///   - type: type of element
    ///   - normalized: flag is element normalized
    BufferElement(const std::string& name,
                  ShaderDataType type,
                  bool normalized = false);
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(BufferElement);
  };
  
  /// This class stores the layout of vertex Buffer in a vector (stores each elements)
  class BufferLayout {
  public:
    /// This is the default constructor
    BufferLayout() = default;
    /// This Costructor initialize the vector of layout elements with
    /// initializer list
    BufferLayout(const std::initializer_list<BufferElement>& elements);
    
    /// This function returns the elements vector
    const std::vector<BufferElement> GetElements() const;
    /// This function returns the stride value
    uint32_t GetStride() const;
    
    // Iterators to access the vector
    std::vector<BufferElement>::iterator begin();
    std::vector<BufferElement>::iterator end();
    std::vector<BufferElement>::const_iterator begin() const;
    std::vector<BufferElement>::const_iterator end() const;
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(BufferLayout);
    
  private:
    // member function
    /// This function calculates the offset of element and update the stride
    /// value
    void CalculateOffsetAndStride();
    
    // Member variable
    std::vector<BufferElement> elements_;
    uint32_t stride_ = 0;
  };
  
  /// This class is the interface of Renderer Vertex Buffer, to store the vertices of the objects.
  class VertexBuffer {
  public:
    // -------------
    // Destrcutor
    // -------------
    virtual ~VertexBuffer() = default;

    // -------------
    // Fundamentals
    // -------------
    /// This function uptate the Buffer layeout value in Vertex Buffer
    /// - Parameter layout: new Buffer layout
    virtual void AddLayout(const BufferLayout& layout) = 0;
    /// This function update the date in buffer dynamically
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    virtual void SetData(void* data, uint32_t size) = 0;
    /// This function binds the Vertex Buffer before rendering
    virtual void Bind() const = 0;
    /// This function unbinds the Vertex Buffer after rendering
    virtual void Unbind() const = 0;

    // -------------
    // Getters
    // -------------
    /// This function returns the Buffer layout stored in Vertex Buffer
    [[nodiscard]] virtual const BufferLayout& GetLayout() const = 0;
    /// This function returns the size of Vertex Buffer in GPU
    [[nodiscard]] virtual uint32_t GetSize() const = 0;
    /// This function returns the renderer ID of Vertex Buffer
    [[nodiscard]] virtual RendererID GetRendererID() const = 0;

    // -----------------
    // Static Function
    // -----------------
    /// This static function creates the instance of Vertex Buffer based on the supported API. This is
    /// static data buffer which takes the data pointer in the argument
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    [[nodiscard]] static std::shared_ptr<VertexBuffer> Create(void* data, uint32_t size);
    /// This static function creates the instance of Vertex Buffer based on the supported API. This is
    /// a dynamic data buffer, takes only size of buffer
    /// - Parameter size: size of data
    [[nodiscard]] static std::shared_ptr<VertexBuffer> Create(uint32_t size);
  };

  /// This class is the interface of Renderer Vertex Buffer, to store the vertices of the objects.
  class IndexBuffer {
  public:
    // -------------
    // Destrcutor
    // -------------
    virtual ~IndexBuffer() = default;
    
    // -------------
    // Fundamentals
    // -------------
    /// This function binds the Index Buffer before rendering
    virtual void Bind() const = 0;
    /// This function unbinds the Index Buffer after rendering
    virtual void Unbind() const = 0;

    // -------------
    // Getters
    // -------------
    /// This function returns the Number of Indices used by this Index Buffer
    [[nodiscard]] virtual uint32_t GetCount() const = 0;
    /// This function returns the size of Index Buffer in GPU
    [[nodiscard]] virtual uint32_t GetSize() const = 0;
    /// This function return the renderer ID of Index Buffer
    [[nodiscard]] virtual RendererID GetRendererID() const = 0;

    // -----------------
    // Static Function
    // -----------------
    /// This static function creates the instance of Index Buffer based on the supported API. This api
    /// takes the data buffer and number of indices
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - count: count of indiced (NOTE: Single indices is taken as uint32_t)
    [[nodiscard]] static std::shared_ptr<IndexBuffer> CreateWithCount(void* data, uint32_t count);
  };

}

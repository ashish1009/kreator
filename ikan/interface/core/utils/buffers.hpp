//
//  buffers.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

namespace ikan {
  
  /// This structure stores the data in a buffer as bytes. Allocate the memory in heap on cosntruction
  /// and de-allocate the memory on destruction
  struct Buffer {
  public:
    // Member variables
    std::byte* data = nullptr;
    uint32_t size = 0;
    
    // Constructors
    constexpr Buffer() = default;
    /// This constructor assign the pointer comming in the argument as 'data'
    /// - Parameters:
    ///   - data: data pointer
    ///   - size: size of data
    Buffer(std::byte* data, uint32_t size);
    /// This is the copy constructor thet deep copies the data from 'other'
    /// - Parameter other: other Buffer
    Buffer(const Buffer& other);
    /// This is the move constructor that moves the buffer from other to this
    /// - Parameter other: other Buffer
    Buffer(Buffer&& other);
    
    // Destructor
    /// This destructor destroy the memory of buffer and clear it
    ~Buffer();
    
    // Member Functions
    /// This functions deletes the Buffer and make size 0
    void Clear();
    /// This function sets the memory of buffer to 0
    void ZeroInitialize();
    /// This funciton writes the data in the buffer
    /// - Parameters:
    ///   - data: Data to be written in the buffer
    ///   - size: Size of data to be copied
    ///   - offset: offset from where data to be copied
    void Write(void* data, uint32_t size, uint32_t offset = 0);
    /// This function allocates the memory to buffer of size 'size'
    /// - Parameter size: size of new buffer
    void Allocate(uint32_t size);
    
    // Getters
    /// This function returns the size of buffer
    constexpr uint32_t GetSize() const;
    
    // Operators
    /// This function returns true if Data is not nullptr
    operator bool() const;
    /// This function returns the byte reference at position index
    /// - Parameter index: ndex of position
    std::byte& operator[](uint32_t index);
    /// This function returns the byte value at position index
    /// - Parameter index : index of position
    std::byte operator[](uint32_t index) const;
    
    /// This function assign the 'other' buffer to this
    /// - Parameter other: assign Buffer other to this Buffer
    /// NOTE: new memory allocation is done (Deep Copy)
    Buffer& operator =(const Buffer& other);
    /// This function moves the 'other' buffer to this
    /// - Parameter other: move Buffer other to this Buffer
    /// NOTE: No new memory allocation is done. All data moved as it is
    Buffer& operator =(Buffer&& other);
    
    // Static Member Functions
    /// This function creates new instance of Buffer. Copy the 'data' and return the newly created buffer
    /// - Parameters:
    ///   - data: Data to be copied
    ///   - size: size of data
    static Buffer Copy(void* data, uint32_t size);
    /// This function returns the total allocated size in entire program
    static uint32_t GetTotalAllocations();
    
    // Templated Functions : Implementation
    /// This function reads the buffer at specific offset as T
    /// - Parameter offset: offset from where buffer to be read
    template<typename T> T& Read(uint32_t offset) { return *(T*)(data + offset); }
    /// This function reads thr buffer from begining as T
    template<typename T> constexpr T* As() { return (T*)data; }
    
  private:
    /// This function dealocate the memory of buffer
    void Dealocate();
    
    // Stores the Total allocation made in program using Buffer class
    static uint32_t allocated_bytes_;
  };
  
}

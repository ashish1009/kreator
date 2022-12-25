//
//  buffers.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "buffers.hpp"
#include "core/utils/asserts.h"

namespace ikan {
  
  uint32_t Buffer::allocated_bytes_ = 0;
  
  Buffer::Buffer(std::byte* data, uint32_t size) : data(data), size(size) {}
  
  Buffer::Buffer(const Buffer& other) : size(other.size) {
    data = new std::byte[size];
    memcpy(data, other.data, size);
    
    allocated_bytes_ += size;
  }
  
  Buffer::Buffer(Buffer&& other) : data(other.data),  size(other.size) {
    other.size = 0;
    other.data = nullptr;
  }
  
  Buffer::~Buffer() {
    Clear();
  }
  
  void Buffer::Clear() {
    if (data)
      Dealocate();
    
    size = 0;
    data = nullptr;
  }
  
  void Buffer::Allocate(uint32_t allocated_size) {
    if (allocated_size == 0)
      return;
    
    Clear();
    size = allocated_size;
    data = new std::byte[size];
    
    allocated_bytes_ += size;
  }
  
  void Buffer::Dealocate() {
    allocated_bytes_ -= size;
    delete[] data;
  }
  
  void Buffer::ZeroInitialize() {
    if (data)
      memset(data, 0, size);
  }
  
  void Buffer::Write(void* new_data, uint32_t allocated_size, uint32_t offset) {
    if (allocated_size == 0)
      return;
    
    IK_CORE_ASSERT(offset + allocated_size <= size or offset > size,
                   "Buffer overflow!");
    memcpy(data + offset, new_data, allocated_size);
  }
  
  constexpr uint32_t Buffer::GetSize() const {
    return size;
  }
  
  Buffer::operator bool() const { return data; }
  
  std::byte& Buffer::operator[](uint32_t index) { return data[index]; }
  std::byte Buffer::operator[](uint32_t index) const { return data[index]; }
  
  Buffer& Buffer::operator =(const Buffer& other) {
    Clear();
    Allocate(other.size);
    Write((void*)other.data, other.size);
    return *this;
  }
  
  Buffer& Buffer::operator =(Buffer&& other) {
    Clear();
    
    size = other.size;
    data = other.data;
    
    other.size = 0;
    other.data = nullptr;
    
    return *this;
  }
  
  Buffer Buffer::Copy(void* data, uint32_t size) {
    Buffer buffer;
    
    if (size == 0)
      return buffer;
    
    buffer.Allocate(size);
    memcpy(buffer.data, data, size);
    return buffer;
  }
  
  uint32_t Buffer::GetTotalAllocations() {
    return allocated_bytes_;
  }
  
} 

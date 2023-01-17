//
//  uuid.hpp
//  ikan
//
//  Created by Ashish . on 30/12/22.
//

#pragma once

namespace ikan {
  
  /// This class generates a "UUID" (universally unique identifier) or GUID is (usually) a 64-bit integer
  /// used to "uniquely" identify information. In Kreator Engine, even though we use the term GUID
  /// and UUID, at the moment we're simply using a randomly generated 64-bit integer, as the possibility
  /// of a clash is low enough for now.
  ///  - Important:
  ///   - Default Constructor generates the random 64bit number
  ///   - Constructor with uint64_t stores already the random generated 64 bit number
  class UUID {
  public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID& other);
    
    /// This function returns the UUID as uint64_t
    operator uint64_t () { return uuid_; }
    /// This function returns the UUID as const uint64_t
    operator const uint64_t () const { return uuid_; }
    
  private:
    uint64_t uuid_;
  };
  
}

namespace std {
  
  template <> struct hash<ikan::UUID> {
    std::size_t operator()(const ikan::UUID& uuid) const {
      return hash<uint64_t>()((uint64_t)uuid);
    }
  };
  
} 

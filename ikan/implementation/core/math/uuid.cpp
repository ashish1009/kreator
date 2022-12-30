//
//  uuid.cpp
//  ikan
//
//  Created by Ashish . on 30/12/22.
//

#include "uuid.hpp"
#include <random>

namespace ikan {
  
  static std::random_device random_device_;           // Will be used to obtain a seed for the random number engine
  static std::mt19937_64 engine_(random_device_());   // Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<uint64_t> uniform_distribution_;
  
  UUID::UUID() : uuid_(uniform_distribution_(engine_)) { }
  
  UUID::UUID(uint64_t uuid) : uuid_(uuid) { }
  
  UUID::UUID(const UUID& other) : uuid_(other.uuid_) { }
  
}

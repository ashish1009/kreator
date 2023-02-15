//
//  entry_point.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

// Including Core entiry point

#define Vulkan 0
#define DebugEntryPoint 0
#if Vulkan
#include "vulkan_entry_point.h"
#elif DebugEntryPoint
int main() {
//  std::vector<int> v = {5, 3, 6, 8, 1, 0};
//  std::vector<int> r;
//
////  r.resize(v.size());
//  for (const auto vvv :v) {
//    const auto insert_pos = std::lower_bound(std::begin(r), std::end(r), vvv);
//    r.insert(insert_pos, vvv);
//  }
//
  
  int *b = nullptr;
  {
    int *a = new int(4);
    b = a;
  }
  
  if (b) {
    std::cout << "not ";
  }
  else {
    std::cout << "bull";
  }
  
  int * c = b;
  delete b;
  
  return 0;
}
#else
#include <core_entry_point.h>
#endif

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
class B {
public:
  B& operator=(const B& o) {
    return *this;
  }
};

class D : public B {
public:
  D& operator=(const D& o) {
    return *this;
  }
};

int main() {
  D* d = new D();
  D* b = new D();
  
  *b = *d;
  
  return 0;
}

class Base {
public:
  virtual void SomeFunction() = 0;
};

class Derived : public Base {
public:
  void SomeFunction() override {}
private:
  int some_variables;
};
 
// Some other Derived classes too

std::vector<Base*> instances;

// Instances have some entries. We keep on pushing entries with time in instance
// After sometime when vector capacity full then vector copy all the data in new memory,
// so copy operator= calls, but only Base class operator= is called which is causing data loss




#else
#include <core_entry_point.h>
#endif

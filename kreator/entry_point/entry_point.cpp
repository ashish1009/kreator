//
//  entry_point.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

// Including Core entiry point 

#define Vulkan 1
#if Vulkan

#include <GLFW/glfw3.h>

int main() {
  std::cout << "vulkan Main ... \n";
  
  assert(GLFW_TRUE == glfwInit());
  
  GLFWwindow* window_ = glfwCreateWindow((int32_t)1600,
                                         (int32_t)900,
                                         "Vulkan",
                                         nullptr, /* External Monitor Pointer : NO
                                                   Use current Selected Monitor */
                                         nullptr  /* Share Monitor : NO */
                                         );

  // If Window is not created successfully then terminate...
  if (window_ == NULL) {
    glfwTerminate();
    assert(false);
  }
  
  // Setting VSync as True
  glfwSwapInterval(1);
  
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
  }

  glfwTerminate();
  
  return 0;
}

#else

#include <core_entry_point.h>

#endif

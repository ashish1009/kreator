//
//  entry_point.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

// Including Core entiry point 

#define Vulkan 1
#if Vulkan

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

int main() {
  std::cout << "vulkan Main ... \n";
  
  bool success =  glfwInit();
  if (!success){
    std::cout << "Failed \n";
  }
  assert(GLFW_TRUE == glfwInit());
  
  GLFWwindow* window_ = glfwCreateWindow((int32_t)800,
                                         (int32_t)800,
                                         "Vulkan",
                                         nullptr, /* External Monitor Pointer : NO
                                                   Use current Selected Monitor */
                                         nullptr  /* Share Monitor : NO */
                                         );
  
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


  // If Window is not created successfully then terminate...
  if (window_ == NULL) {
    glfwTerminate();
    assert(false);
  }
  
  // Setting VSync as True
  glfwSwapInterval(1);
  
  auto t = glfwVulkanSupported();
//  assert(glfwVulkanSupported());

  
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
  }

  glfwTerminate();
  
  return 0;
}

#else

#include <core_entry_point.h>

#endif

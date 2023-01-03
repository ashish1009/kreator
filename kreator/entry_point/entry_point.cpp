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
//#include <Availability.h>
//
//#define VK_USE_PLATFORM_METAL_EXT        1
//
//#define VK_ENABLE_BETA_EXTENSIONS        1    // VK_KHR_portability_subset
//
//#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
//#  define VK_USE_PLATFORM_IOS_MVK        1
//#endif
//
//#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
//#  define VK_USE_PLATFORM_MACOS_MVK      1
//#endif

#include <vulkan/vulkan.h>

inline static VkInstance s_VulkanInstance;

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
  
  assert(glfwVulkanSupported());
  
  VkApplicationInfo app_info{};                                // APPLICATION INFO
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;         // type of the struct
  app_info.pApplicationName = "ikan";                          // name of the application
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);      // version
  app_info.pEngineName = "ikan";              // name of the engine
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);           // version
  app_info.apiVersion = VK_API_VERSION_1_0;                    // API version
  
  // setup create info struct
  VkInstanceCreateInfo instance_create_info{};                           // INSTANCE CREATION INFO
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;   // type of struct
  instance_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  instance_create_info.pApplicationInfo = &app_info;                     // application info from above
  
  std::vector<const char*> instanceExtensions = { VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME };
//  instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
//  instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Very little performance hit, can be used in Release.
  
  {
//    instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
//    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  }
  
  instance_create_info.enabledExtensionCount = 0;//(uint32_t)instanceExtensions.size();      // amount of extensions to be enabled
  instance_create_info.ppEnabledExtensionNames = instanceExtensions.data();        // extensions to enable
  
  const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
  instance_create_info.enabledLayerCount = 0;//enable_validation_layers ? static_cast<uint32_t>(validation_layers.size()) : 0; // validation layer count
  instance_create_info.ppEnabledLayerNames = &validationLayerName;//enable_validation_layers ? validation_layers.data() : nullptr;                // validation layer names
  
  // try creating instance, catch potential error code
  VkResult creation_result = VK_RESULT_MAX_ENUM;
  creation_result = vkCreateInstance(&instance_create_info, nullptr, &s_VulkanInstance);
  
  if (creation_result != VK_SUCCESS) {
    assert(true);
  }
  
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
  }
  
  glfwTerminate();
  
  return 0;
}

#else

#include <core_entry_point.h>

#endif

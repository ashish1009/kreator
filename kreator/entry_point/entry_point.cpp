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

inline static VkInstance s_VulkanInstance;
inline static VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
inline static VkDevice device;
inline static VkQueue graphicsQueue;
inline static VkSurfaceKHR surface;
inline static VkQueue presentQueue;

std::vector<const char*> getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  
  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  
  if (true) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  
  return extensions;
}

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  
  bool isComplete() {
    return graphicsFamily.has_value();
  }
};


QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
  
  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    
    if (indices.isComplete()) {
      break;
    }
    
    i++;
  }
  
  return indices;
}


bool isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = findQueueFamilies(device);
  
  return indices.isComplete();
}

int main() {
  std::cout << "vulkan Main ... \n";
  
  bool success =  glfwInit();
  if (!success){
    std::cout << "Failed \n";
  }
  assert(GLFW_TRUE == glfwInit());
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  
  GLFWwindow* window_ = glfwCreateWindow((int32_t)800,
                                         (int32_t)800,
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
  
  uint32_t extenstion_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extenstion_count, nullptr);
  
  uint32_t layer_count = 0;
  VkExtensionProperties p;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  
  std::cout << "extenstion_count " << extenstion_count << " layer " << layer_count  << "\n";

  assert(glfwVulkanSupported());
  
  //---------------------------
  // Create Instance
  //---------------------------
  VkApplicationInfo app_info{};                                // APPLICATION INFO
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;         // type of the struct
  app_info.pApplicationName = "ikan";                          // name of the application
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);      // version
  app_info.pEngineName = "ikan";              // name of the engine
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);           // version
  app_info.apiVersion = VK_API_VERSION_1_3;                    // API version
  
  // setup create info struct
  VkInstanceCreateInfo instance_create_info{};                           // INSTANCE CREATION INFO
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;   // type of struct
  instance_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  instance_create_info.pApplicationInfo = &app_info;                     // application info from above
  
  auto extensions = getRequiredExtensions();
  instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  instance_create_info.ppEnabledExtensionNames = extensions.data();
  
  const char* validationLayerName = "VK_KHR_surface";
  instance_create_info.enabledLayerCount = 1;
  instance_create_info.ppEnabledLayerNames =&validationLayerName ;
  instance_create_info.pNext = nullptr;
    
  // try creating instance, catch potential error code
  VkResult creation_result = VK_RESULT_MAX_ENUM;
  creation_result = vkCreateInstance(&instance_create_info, nullptr, &s_VulkanInstance);
    
  if (creation_result != VK_SUCCESS) {
    assert(true);
  }
  
  // --------------------------
  // Create Surface
  // --------------------------
  if (glfwCreateWindowSurface(s_VulkanInstance, window_, nullptr, &surface) != VK_SUCCESS) {

  }


  // ---------------------------------
  // Pick Physical device
  // ---------------------------------
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(s_VulkanInstance, &deviceCount, nullptr);
  
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(s_VulkanInstance, &deviceCount, devices.data());
  
  for (const auto& device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      break;
    }
  }
  
  if (physicalDevice == VK_NULL_HANDLE) {
    
  }
  
  // ------------------------
  // Create Logical Device
  // ------------------------
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
  
  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;
  
  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;
  
  VkPhysicalDeviceFeatures deviceFeatures{};
  
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  
  createInfo.pEnabledFeatures = &deviceFeatures;
  
  createInfo.enabledExtensionCount = 0;
  
  createInfo.enabledLayerCount = 0;
  
  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {

  }
  
  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);


  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
  }
  
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(s_VulkanInstance, nullptr);
  glfwDestroyWindow(window_);
  glfwTerminate();

  return 0;
}

#else

#include <core_entry_point.h>

#endif

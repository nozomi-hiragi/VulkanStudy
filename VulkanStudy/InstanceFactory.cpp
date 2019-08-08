
#include "InstanceFactory.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

constexpr auto ENGINE_NAME = "Speell";
constexpr auto ENGINE_VERSION = 0;

const VkInstance InstanceFactory::_createVkInstance(const char* app_name, const uint32_t app_version) {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = app_version;
  app_info.pEngineName = ENGINE_NAME;
  app_info.engineVersion = ENGINE_VERSION;
  app_info.apiVersion = VK_API_VERSION_1_0;

  // Layers verification
  std::vector<const char*> required_instance_layers;
#ifdef _DEBUG
  required_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // DEBUG

  uint32_t layer_count = 0;
  auto result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_instance_layers(layer_count);
  result = vkEnumerateInstanceLayerProperties(&layer_count, available_instance_layers.data());

  std::vector<const char*> enabled_instance_layers;
  for (auto required : required_instance_layers) {
    for (auto allowed : available_instance_layers) {
      if (strcmp(required, allowed.layerName) == 0) {
        enabled_instance_layers.push_back(required);
        break;
      }
    }
  }

  // Extensions verification
  uint32_t extensions_count = 0;
  const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  instance_info.enabledLayerCount = static_cast<uint32_t>(enabled_instance_layers.size());
  instance_info.ppEnabledLayerNames = enabled_instance_layers.data();
  instance_info.enabledExtensionCount = extensions_count;
  instance_info.ppEnabledExtensionNames = extensions;

  VkInstance instance;
  result = vkCreateInstance(&instance_info, nullptr, &instance);
  return instance;
}


#include "InstanceFactory.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

constexpr auto ENGINE_NAME = "Speell";
constexpr auto ENGINE_VERSION = 0;

const VkInstance InstanceFactory::_createVkInstance(const char* app_name, const uint32_t app_version, const std::vector<const char*>& extensions) {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = app_version;
  app_info.pEngineName = ENGINE_NAME;
  app_info.engineVersion = ENGINE_VERSION;
  app_info.apiVersion = VK_API_VERSION_1_0;

  std::vector<const char*> required_instance_layers;
  std::vector<const char*> required_instance_extensions;

#ifdef _DEBUG
  required_instance_layers.push_back("VK_LAYER_GOOGLE_threading");
  required_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
  required_instance_layers.push_back("VK_LAYER_LUNARG_parameter_validation");
  required_instance_layers.push_back("VK_LAYER_LUNARG_object_tracker");
  required_instance_layers.push_back("VK_LAYER_LUNARG_image");
  required_instance_layers.push_back("VK_LAYER_LUNARG_core_validation");
  required_instance_layers.push_back("VK_LAYER_LUNARG_swapchain");
  required_instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif // DEBUG

  uint32_t glfw_extensions_count = 0;
  const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
  for (uint32_t i = 0; i < glfw_extensions_count; i++) {
    required_instance_extensions.push_back(glfw_extensions[i]);
  }
  required_instance_extensions.reserve(required_instance_extensions.size() + extensions.size());
  std::copy(extensions.begin(), extensions.end(), std::back_inserter(required_instance_extensions));

  // Layers verification
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
  uint32_t extension_count = 0;
  result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_instance_extensions(extension_count);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_instance_extensions.data());
  std::vector<const char*> enabled_instance_extensions;
  for (auto required : required_instance_extensions) {
    for (auto allowed : available_instance_extensions) {
      if (strcmp(required, allowed.extensionName) == 0) {
        enabled_instance_extensions.push_back(required);
        break;
      }
    }
  }

  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  instance_info.enabledLayerCount = static_cast<uint32_t>(enabled_instance_layers.size());
  instance_info.ppEnabledLayerNames = enabled_instance_layers.data();
  instance_info.enabledExtensionCount = static_cast<uint32_t>(enabled_instance_extensions.size());
  instance_info.ppEnabledExtensionNames = enabled_instance_extensions.data();

  VkInstance vk_instance;
  result = vkCreateInstance(&instance_info, nullptr, &vk_instance);

  return vk_instance;
}

#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

constexpr auto ENGINE_NAME = "Speell";
constexpr auto ENGINE_VERSION = 0;

class Instance {
public:
  Instance() : _instance(nullptr) {
  }

  ~Instance() {
  }

  void createInstance(const char* app_name, uint32_t app_version) {
    const auto app_info = vk::ApplicationInfo()
      .setPApplicationName(app_name)
      .setApplicationVersion(app_version)
      .setPEngineName(ENGINE_NAME)
      .setEngineVersion(ENGINE_VERSION)
      .setApiVersion(VK_API_VERSION_1_0);

    // Layers verification
    std::vector<const char*> required_instance_layers;
#ifdef _DEBUG
    required_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // DEBUG
    std::vector<const char*> enabled_instance_layers;
    auto allowed_instance_layers = vk::enumerateInstanceLayerProperties();
    for (auto required : required_instance_layers) {
      for (auto allowed : allowed_instance_layers) {
        if (strcmp(required, allowed.layerName) == 0) {
          enabled_instance_layers.push_back(required);
          break;
        }
      }
    }

    // Extensions verification
    std::vector<const char*> required_instance_extensions;
    required_instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    required_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
    std::vector<const char*> enabled_instance_extensions;
    auto allowed_instance_extensions = vk::enumerateInstanceExtensionProperties();
    for (auto required : required_instance_extensions) {
      for (auto allowed : allowed_instance_extensions) {
        if (strcmp(required, allowed.extensionName) == 0) {
          enabled_instance_extensions.push_back(required);
          break;
        }
      }
    }

    const auto instance_info = vk::InstanceCreateInfo()
      .setPApplicationInfo(&app_info)
      .setEnabledLayerCount(static_cast<uint32_t>(enabled_instance_layers.size()))
      .setPpEnabledLayerNames(enabled_instance_layers.data())
      .setEnabledExtensionCount(static_cast<uint32_t>(enabled_instance_extensions.size()))
      .setPpEnabledExtensionNames(enabled_instance_extensions.data());

    _instance = vk::createInstance(instance_info);
  }

  void destroyInstance() {
    if (!_instance) { return; }
    _instance.destroy();
    _instance = nullptr;
  }

  vk::SurfaceKHR createSurface(HINSTANCE hinstance, HWND hwnd) {
    return _instance.createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR()
      .setHinstance(hinstance)
      .setHwnd(hwnd));
  }

  void destroySurface(vk::SurfaceKHR surface) {
    if (!surface) { return; }
    _instance.destroySurfaceKHR(surface);
  }

  std::vector<vk::PhysicalDevice> getPhysicalDevices() {
    return _instance.enumeratePhysicalDevices();
  }

protected:

private:
  vk::Instance _instance;
};

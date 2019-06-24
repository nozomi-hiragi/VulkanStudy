#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include "PhysicalDevice.h"
#include "Surface.h"

constexpr auto ENGINE_NAME = "Speell";
constexpr auto ENGINE_VERSION = 0;

class Instance {
public:
  Instance(): _instance(nullptr) {
  }

  ~Instance() {
  }

  void createInstance(const char* app_name, const uint32_t app_version) {
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

  Surface createSurface(const HINSTANCE hinstance, const HWND hwnd) {
    return Surface(_instance.createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR()
      .setHinstance(hinstance)
      .setHwnd(hwnd)));
  }

  void destroySurface(Surface& surface) {
    auto vk_surface = surface.getVkSurface();
    if (!vk_surface) { return; }
    _instance.destroySurfaceKHR(vk_surface);
    surface.setVkSurface(nullptr);
  }

  PhysicalDevice getPhysicalDevice(const size_t index) {
    if (_physical_devices.size() <= 0) {
      _physical_devices = std::move(_instance.enumeratePhysicalDevices());
    }
    return PhysicalDevice(_physical_devices[index]);
  }

protected:

private:
  vk::Instance _instance;
  std::vector<vk::PhysicalDevice> _physical_devices;
};

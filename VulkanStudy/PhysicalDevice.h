#pragma once

#include <vulkan/vulkan.h>

#include <set>

#include "Surface.h"
#include "Device.h"

class PhysicalDeviceObject {
public:
  PhysicalDeviceObject(const VkPhysicalDevice physical_device) : _physical_device(physical_device) {
  }

  ~PhysicalDeviceObject() {
  }

  const VkPhysicalDevice _physical_device;
protected:
private:
};

class PhysicalDeviceFactory {
public:
  PhysicalDeviceFactory() {
  }

  ~PhysicalDeviceFactory() {
  }

protected:
private:
  std::set<std::shared_ptr<PhysicalDeviceObject>> _container;
};

class PhysicalDevice {
public:
  PhysicalDevice(): _physical_device(nullptr) {
  }

  PhysicalDevice(const VkPhysicalDevice physical_device):
    _physical_device(physical_device),
    _present_queue_family_index(UINT32_MAX) {

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, queue_family_properties.data());

    _queue_family_properties.swap(queue_family_properties);

    vkGetPhysicalDeviceMemoryProperties(_physical_device, &_memory_properties);
  }

  ~PhysicalDevice() {
  }

  // for memory
  VkPhysicalDeviceMemoryProperties getMemoryProperties() {
    return _memory_properties;
  }

  // for queue
  std::vector<VkQueueFamilyProperties> getQueueFamilyProperties() {
    return _queue_family_properties;
  }

  auto findPresentQueueFamilyIndex(Surface& surface) {
    if (_present_queue_family_index != UINT32_MAX) {
      return _present_queue_family_index;
    }

    for (uint32_t i = 0; i < _queue_family_properties.size(); i++) {
      auto is_graphycs_queue_family = (_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT;
      auto is_support_surface = getSurfaceSupport(i, surface);
      if (is_graphycs_queue_family && is_support_surface) {
        _present_queue_family_index = i;
        break;
      }
    }
    return _present_queue_family_index; // ‚±‚±‚ÅƒGƒ‰[“f‚©‚¹‚éH
  }

  bool getSurfaceSupport(const uint32_t index, Surface& surface) {
    VkBool32 result;
    vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, index, surface.getVkSurface(), &result);
    return result == VK_TRUE;
  }

  // for swapchain
  VkSurfaceCapabilitiesKHR getSurfaceCapabilities(Surface& surface) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, surface.getVkSurface(), &surface_capabilities);
    return surface_capabilities;
  }

  std::vector<VkSurfaceFormatKHR> getSurfaceFormats(Surface& surface) {
    uint32_t count = 0;
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, surface.getVkSurface(), &count, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(count);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, surface.getVkSurface(), &count, surface_formats.data());

    return std::move(surface_formats);
  }

  // create device
  Device createDevice(Surface& surface) {
    _present_queue_family_index = findPresentQueueFamilyIndex(surface);

    float queue_prioritie = 0.f;
    VkDeviceQueueCreateInfo queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueCount = 1;
    queue_info.queueFamilyIndex = _present_queue_family_index;
    queue_info.pQueuePriorities = &queue_prioritie;

    // Extensions verification
    std::vector<const char*> required_device_extensions;
    required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    uint32_t extension_count = 0;
    auto result = vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_device_extensions(extension_count);
    result = vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &extension_count, available_device_extensions.data());


    std::vector<const char*> enabled_device_extensions;
    for (auto required : required_device_extensions) {
      for (auto allowed : available_device_extensions) {
        if (strcmp(required, allowed.extensionName) == 0) {
          enabled_device_extensions.push_back(required);
          break;
        }
      }
    }

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = nullptr;
    device_info.enabledExtensionCount = static_cast<uint32_t>(enabled_device_extensions.size());
    device_info.ppEnabledExtensionNames = enabled_device_extensions.data();
    device_info.pEnabledFeatures = nullptr;

    VkDevice device;
    vkCreateDevice(_physical_device, &device_info, nullptr, &device);

    return Device(device, _present_queue_family_index);
  }

  static auto getBlankPhysicalDevice() {
    static PhysicalDevice blank_physical_device;
    return blank_physical_device;
  }

protected:

private:
  VkPhysicalDevice _physical_device;
  std::vector<VkQueueFamilyProperties> _queue_family_properties;
  VkPhysicalDeviceMemoryProperties _memory_properties;
  uint32_t _present_queue_family_index;
};

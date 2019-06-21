#pragma once

#include <vulkan/vulkan.hpp>

class PhysicalDevice {
public:
  PhysicalDevice() : _physical_device(nullptr) {
  }

  PhysicalDevice(vk::PhysicalDevice physical_device) : _physical_device(physical_device) {
  }

  ~PhysicalDevice() {
  }

  // for memory
  vk::PhysicalDeviceMemoryProperties getMemoryProperties() {
    return _physical_device.getMemoryProperties();
  }

  // for queue
  std::vector<vk::QueueFamilyProperties> getQueueFamilyProperties() {
    return _physical_device.getQueueFamilyProperties();
  }

  bool getSurfaceSupport(uint32_t index, vk::SurfaceKHR surface) {
    return _physical_device.getSurfaceSupportKHR(index, surface) == VK_TRUE;
  }

  // for swapchain
  vk::SurfaceCapabilitiesKHR getSurfaceCapabilities(vk::SurfaceKHR surface) {
    return _physical_device.getSurfaceCapabilitiesKHR(surface);
  }

  std::vector<vk::SurfaceFormatKHR> getSurfaceFormats(vk::SurfaceKHR surface) {
    return _physical_device.getSurfaceFormatsKHR(surface);
  }

  // create device
  vk::Device createDevice(vk::DeviceQueueCreateInfo& queue_info) {
    // Extensions verification
    std::vector<const char*> required_device_extensions;
    required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    std::vector<const char*> enabled_device_extensions;
    auto allowed_device_extensions = _physical_device.enumerateDeviceExtensionProperties();
    for (auto required : required_device_extensions) {
      for (auto allowed : allowed_device_extensions) {
        if (strcmp(required, allowed.extensionName) == 0) {
          enabled_device_extensions.push_back(required);
          break;
        }
      }
    }

    const auto device_info = vk::DeviceCreateInfo()
      .setQueueCreateInfoCount(1)
      .setPQueueCreateInfos(&queue_info)
      .setEnabledLayerCount(0)
      .setPpEnabledLayerNames(nullptr)
      .setEnabledExtensionCount(static_cast<uint32_t>(enabled_device_extensions.size()))
      .setPpEnabledExtensionNames(enabled_device_extensions.data())
      .setPEnabledFeatures(nullptr);

    return _physical_device.createDevice(device_info);
  }

protected:

private:
  vk::PhysicalDevice _physical_device;
};

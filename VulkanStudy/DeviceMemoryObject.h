#pragma once

#include <vulkan/vulkan.h>

class DeviceMemoryObject {
public:
  DeviceMemoryObject(const VkDeviceMemory device_memory) : _vk_device_memory(device_memory) {
  }

  ~DeviceMemoryObject() {
  }

  const VkDeviceMemory _vk_device_memory;
protected:
private:
};

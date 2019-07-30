#pragma once

#include <vulkan/vulkan.h>

#include "DeviceObject.h"

class DeviceMemoryObject {
public:
  DeviceMemoryObject(const VkDeviceMemory device_memory) : _vk_device_memory(device_memory) {
  }

  ~DeviceMemoryObject() {
  }

  void* mapMemory(const std::shared_ptr<DeviceObject> device, const uint64_t offset, const uint64_t size) {
    void* data;
    vkMapMemory(device->_vk_device, _vk_device_memory, offset, size, 0, &data);
    return data;
  }

  void unmapMemory(const std::shared_ptr<DeviceObject> device) {
    vkUnmapMemory(device->_vk_device, _vk_device_memory);
  }

  const VkDeviceMemory _vk_device_memory;
protected:
private:
};

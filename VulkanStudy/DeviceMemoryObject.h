#pragma once

#include <vulkan/vulkan.h>

class DeviceMemoryObject {
public:
  DeviceMemoryObject(const VkDeviceMemory device_memory) : _vk_device_memory(device_memory) {
  }

  ~DeviceMemoryObject() {
  }

  static void* vkMapMemory_(VkDevice device, const vk::DeviceMemory memory, const uint64_t offset, const uint64_t size) {
    void* data;
    vkMapMemory(device, memory, offset, size, 0, &data);
    return data;
  }

  static void vkUnmapMemory_(VkDevice device, const vk::DeviceMemory memory) {
    vkUnmapMemory(device, memory);
  }

  const VkDeviceMemory _vk_device_memory;
protected:
private:
};

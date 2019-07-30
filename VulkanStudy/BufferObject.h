#pragma once

#include <vulkan/vulkan.h>

#include "DeviceObject.h"
#include "DeviceMemoryObject.h"

class BufferObject {
public:
  BufferObject(const VkBuffer buffer, const VkMemoryRequirements memory_requirements):
    _vk_buffer(buffer),
    _vk_memory_requirements(memory_requirements) {
  }

  ~BufferObject() {
  }

  void bindBufferMemory(const std::shared_ptr<DeviceObject> device, const std::shared_ptr<DeviceMemoryObject> memory, const uint64_t offset) {
    vkBindBufferMemory(device->_vk_device, _vk_buffer, memory->_vk_device_memory, offset);
  }

  const VkBuffer _vk_buffer;
  const VkMemoryRequirements _vk_memory_requirements;
protected:
private:
};

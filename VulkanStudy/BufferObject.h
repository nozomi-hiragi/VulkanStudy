#pragma once

#include <vulkan/vulkan.h>

class BufferObject {
public:
  BufferObject(const VkBuffer buffer, const VkMemoryRequirements memory_requirements):
    _vk_buffer(buffer),
    _vk_memory_requirements(memory_requirements) {
  }

  ~BufferObject() {
  }

  static void vkBindBufferMemory_(VkDevice device, const VkBuffer buffer, const VkDeviceMemory memory, const uint64_t offset) {
    vkBindBufferMemory(device, buffer, memory, offset);
  }

  const VkBuffer _vk_buffer;
  const VkMemoryRequirements _vk_memory_requirements;
protected:
private:
};

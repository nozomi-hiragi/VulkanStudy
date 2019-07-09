#pragma once

#include <vulkan/vulkan.h>

class BufferObject {
public:
  BufferObject(const VkBuffer buffer, const VkMemoryRequirements memory_requirements):
    _vk_buffer(buffer),
    _memory_requirements(memory_requirements) {
  }

  ~BufferObject() {
  }

  const VkBuffer _vk_buffer;
  const VkMemoryRequirements _memory_requirements;
protected:
private:
};

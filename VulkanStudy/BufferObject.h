#pragma once

#include <vulkan/vulkan.h>

class BufferObject {
public:
  BufferObject(const VkBuffer buffer) : _vk_buffer(buffer) {
  }

  ~BufferObject() {
  }

  const VkBuffer _vk_buffer;
protected:
private:
};

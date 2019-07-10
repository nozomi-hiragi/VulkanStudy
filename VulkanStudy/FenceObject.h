#pragma once

#include <vulkan/vulkan.h>

class FenceObject {
public:
  FenceObject(VkFence fence) : _vk_fence(fence) {
  }

  ~FenceObject() {
  }

  static VkResult vkWaitForFence(VkDevice device, VkFence fence, uint64_t timeout) {
    return vkWaitForFences(device, 1, &fence, VK_FALSE, timeout);
  }

  const VkFence _vk_fence;
protected:
private:
};

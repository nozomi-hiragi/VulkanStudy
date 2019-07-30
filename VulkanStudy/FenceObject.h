#pragma once

#include <vulkan/vulkan.h>
#include <memory>

class DeviceObject;

class FenceObject {
public:
  FenceObject(VkFence fence) : _vk_fence(fence) {
  }

  ~FenceObject() {
  }

  VkResult waitForFence(const std::shared_ptr<DeviceObject> device, uint64_t timeout);

  void resetFence(const std::shared_ptr<DeviceObject> device);

  const VkFence _vk_fence;
protected:
private:
};

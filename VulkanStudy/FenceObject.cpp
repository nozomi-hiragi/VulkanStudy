
#include "FenceObject.h"
#include "DeviceObject.h"

VkResult FenceObject::waitForFence(const std::shared_ptr<DeviceObject> device, uint64_t timeout) {
  return vkWaitForFences(device->_vk_device, 1, &_vk_fence, VK_FALSE, timeout);
}

void FenceObject::resetFence(const std::shared_ptr<DeviceObject> device) {
  vkResetFences(device->_vk_device, 1, &_vk_fence);
}

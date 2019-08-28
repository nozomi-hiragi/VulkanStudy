#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "StandardFactory.h"
#include "FenceObject.h"
#include "DeviceObject.h"

class FenceFactory : public StandardFactory<FenceObject, DeviceObject> {
public:
protected:
private:
  static auto _createVkFence(VkDevice device) {
    VkFenceCreateInfo fence_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

    VkFence fence;
    auto result = vkCreateFence(device, &fence_info, nullptr, &fence);
    return fence;
  }

  static void _destroyVkFence(VkDevice device, VkFence fence) {
    vkDestroyFence(device, fence, nullptr);
  }

  std::shared_ptr<FenceObject> _createCore() {
    auto vk_fence = _createVkFence(_parent->_vk_device);
    return std::make_shared<FenceObject>(vk_fence);
  }

  void _destroyCore(std::shared_ptr<FenceObject> object) {
    object->waitForFence(_parent, UINT32_MAX);
    _destroyVkFence(_parent->_vk_device, object->_vk_fence);
  }

};

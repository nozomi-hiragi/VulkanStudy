#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "FenceObject.h"

class FenceFactory {
  static auto _createVkFence(VkDevice device) {
    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence fence;
    auto result = vkCreateFence(device, &fence_info, nullptr, &fence);
    return fence;
  }

  static void _destroyVkFence(VkDevice device, VkFence fence) {
    vkDestroyFence(device, fence, nullptr);
  }

public:
  FenceFactory() {
  }

  ~FenceFactory() {
  }

  auto createFence(VkDevice device) {
    auto vk_fence = _createVkFence(device);
    auto object = std::make_shared<FenceObject>(vk_fence);
    _container.insert(object);
    return object;
  }

  void destroyFence(VkDevice device, std::shared_ptr<FenceObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      FenceObject::vkWaitForFence(device, object->_vk_fence, UINT32_MAX);
      _destroyVkFence(device, object->_vk_fence);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<FenceObject>> _container;
};

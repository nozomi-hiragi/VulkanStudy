#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>
#include <array>

#include "AbstractFactory.h"
#include "DescriptorPoolObject.h"
#include "DeviceObject.h"

class DescriptorPoolFactory : public AbstractFactory<DescriptorPoolObject, DeviceObject> {
public:
protected:
private:
  static VkDescriptorPool _createVkDescriptorPool(VkDevice device) {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
      { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    };

    VkDescriptorPoolCreateInfo descriptor_pool_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    descriptor_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptor_pool_info.maxSets = 6000;
    descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    descriptor_pool_info.pPoolSizes = pool_sizes.data();

    VkDescriptorPool descriptor_pool;
    auto result = vkCreateDescriptorPool(device, &descriptor_pool_info, nullptr, &descriptor_pool);
    return descriptor_pool;
  }

  static void _destroyVkDescriptorPool(VkDevice device, const  VkDescriptorPool descriptor_pool) {
    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
  }

  std::shared_ptr<DescriptorPoolObject> _createCore() {
    auto vk_descriptor_pool = _createVkDescriptorPool(_parent->_vk_device);
    return std::make_shared<DescriptorPoolObject>(vk_descriptor_pool);
  }

  void _destroyCore(std::shared_ptr<DescriptorPoolObject> object) {
    _destroyVkDescriptorPool(_parent->_vk_device, object->_vk_descriptor_pool);
  }

};

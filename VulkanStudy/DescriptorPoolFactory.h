#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "DescriptorPoolObject.h"

class DescriptorPoolFactory {
  static VkDescriptorPool _createVkDescriptorPool(VkDevice device) {
    VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1 } };

    VkDescriptorPoolCreateInfo descriptor_pool_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    descriptor_pool_info.maxSets = 1;
    descriptor_pool_info.poolSizeCount = 1;
    descriptor_pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool descriptor_pool;
    auto result = vkCreateDescriptorPool(device, &descriptor_pool_info, nullptr, &descriptor_pool);
    return descriptor_pool;
  }

  static void _destroyVkDescriptorPool(VkDevice device, const  VkDescriptorPool descriptor_pool) {
    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
  }

  std::shared_ptr<DescriptorPoolObject> _createCore(VkDevice device) {
    auto vk_descriptor_pool = _createVkDescriptorPool(device);
    return std::make_shared<DescriptorPoolObject>(vk_descriptor_pool);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<DescriptorPoolObject> object) {
    _destroyVkDescriptorPool(device, object->_vk_descriptor_pool);
  }

public:
  std::shared_ptr<DescriptorPoolObject> createObject(VkDevice device) {
    auto object = _createCore(device);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<DescriptorPoolObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyCore(device, object);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<DescriptorPoolObject>> _container;
};

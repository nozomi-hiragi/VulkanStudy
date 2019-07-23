#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "DescriptorSetObject.h"

class DescriptorPoolObject {
  static VkDescriptorSet _allocateVkDescriptorSet(VkDevice device, VkDescriptorPool descriptor_pool, VkDescriptorSetLayout descriptor_set_layout) {
    VkDescriptorSetAllocateInfo descriptor_set_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    descriptor_set_info.descriptorPool = descriptor_pool;
    descriptor_set_info.descriptorSetCount = 1;
    descriptor_set_info.pSetLayouts = &descriptor_set_layout;

    VkDescriptorSet descriptor_set;
    vkAllocateDescriptorSets(device, &descriptor_set_info, &descriptor_set);
    return descriptor_set;
  }

  static void _freeVkDescriptoirSet(VkDevice device, VkDescriptorPool descriptor_pool , VkDescriptorSet descriptor_set) {
    vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_set);
  }

  std::shared_ptr<DescriptorSetObject> _createCore(VkDevice device, VkDescriptorSetLayout descriptor_set_layout) {
    auto vk_pipeline_layout = _allocateVkDescriptorSet(device, _vk_descriptor_pool, descriptor_set_layout);
    return std::make_shared<DescriptorSetObject>(vk_pipeline_layout);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<DescriptorSetObject> object) {
    _freeVkDescriptoirSet(device, _vk_descriptor_pool, object->_vk_descriptor_set);
  }

public:
  DescriptorPoolObject(const VkDescriptorPool descriptor_pool) : _vk_descriptor_pool(descriptor_pool) {
  }

  std::shared_ptr<DescriptorSetObject> createObject(VkDevice device, VkDescriptorSetLayout descriptor_set_layout) {
    auto object = _createCore(device, descriptor_set_layout);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<DescriptorSetObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyCore(device, object);
      object.reset();
    }
  }

  const VkDescriptorPool _vk_descriptor_pool;
protected:
private:
  std::set<std::shared_ptr<DescriptorSetObject>> _container;
};

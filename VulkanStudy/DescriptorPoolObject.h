#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "DescriptorSetObject.h"
#include "DeviceObject.h"
#include "DescriptorSetLayoutObject.h"

class DescriptorPoolObject : public AbstractFactory<DescriptorSetObject, DeviceObject, const std::shared_ptr<DescriptorSetLayoutObject>> {
public:
  DescriptorPoolObject(const VkDescriptorPool descriptor_pool) : _vk_descriptor_pool(descriptor_pool) {
  }

  const VkDescriptorPool _vk_descriptor_pool;
protected:
private:
  static VkDescriptorSet _allocateVkDescriptorSet(VkDevice device, VkDescriptorSetAllocateInfo& descriptor_set_info) {

    VkDescriptorSet descriptor_set;
    vkAllocateDescriptorSets(device, &descriptor_set_info, &descriptor_set);
    return descriptor_set;
  }

  static void _freeVkDescriptoirSet(VkDevice device, VkDescriptorPool descriptor_pool , VkDescriptorSet descriptor_set) {
    vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_set);
  }

  std::shared_ptr<DescriptorSetObject> _createCore(const std::shared_ptr<DescriptorSetLayoutObject> descriptor_set_layout) {
    VkDescriptorSetAllocateInfo descriptor_set_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    descriptor_set_info.descriptorPool = _vk_descriptor_pool;
    descriptor_set_info.descriptorSetCount = 1;
    descriptor_set_info.pSetLayouts = &descriptor_set_layout->_vk_descriptor_set_layout;

    auto vk_pipeline_layout = _allocateVkDescriptorSet(_parent->_vk_device, descriptor_set_info);
    return std::make_shared<DescriptorSetObject>(vk_pipeline_layout, descriptor_set_layout->_descriptor_set_layout_binding_depot);
  }

  void _destroyCore(std::shared_ptr<DescriptorSetObject> object) {
    _freeVkDescriptoirSet(_parent->_vk_device, _vk_descriptor_pool, object->_vk_descriptor_set);
  }

};

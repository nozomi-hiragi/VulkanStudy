#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "StandardFactory.h"
#include "DescriptorSetLayoutObject.h"
#include "DeviceObject.h"

class DescriptorSetLayoutFactory : public StandardFactory<DescriptorSetLayoutObject, DeviceObject, const std::vector<VkDescriptorSetLayoutBinding>&> {
public:

protected:
private:
  static VkDescriptorSetLayout _createVkDescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo& descriptor_set_layout_info) {
    VkDescriptorSetLayout descriptor_set_layout;
    vkCreateDescriptorSetLayout(device, &descriptor_set_layout_info, nullptr, &descriptor_set_layout);
    return descriptor_set_layout;
  }

  static void _destroyVkDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayout descriptor_set_layout) {
    vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
  }

  std::shared_ptr<DescriptorSetLayoutObject> _createCore(const std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_bindings) {
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = descriptor_set_layout_bindings.data();

    auto vk_descriptoir_set_layout = _createVkDescriptorSetLayout(_parent->_vk_device, descriptor_set_layout_info);
    return std::make_shared<DescriptorSetLayoutObject>(vk_descriptoir_set_layout, std::move(descriptor_set_layout_bindings));
  }

  void _destroyCore(std::shared_ptr<DescriptorSetLayoutObject> object) {
    _destroyVkDescriptorSetLayout(_parent->_vk_device, object->_vk_descriptor_set_layout);
  }
};

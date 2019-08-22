#pragma once

#include <vulkan/vulkan.h>

class DescriptorSetLayoutBindingDepot;

class DescriptorSetLayoutObject {
public:
  DescriptorSetLayoutObject(VkDescriptorSetLayout descriptor_set_layout, std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings)
    :_vk_descriptor_set_layout(descriptor_set_layout)
    ,_vk_descriptor_set_layout_bindings(std::move(descriptor_set_layout_bindings)) {
  }

  const VkDescriptorSetLayout _vk_descriptor_set_layout;
  const std::vector<VkDescriptorSetLayoutBinding> _vk_descriptor_set_layout_bindings;
protected:
private:
};

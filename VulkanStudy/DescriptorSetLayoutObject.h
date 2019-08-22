#pragma once

#include <vulkan/vulkan.h>

class DescriptorSetLayoutBindingDepot;

class DescriptorSetLayoutObject {
public:
  DescriptorSetLayoutObject(VkDescriptorSetLayout descriptor_set_layout, DescriptorSetLayoutBindingDepot& descriptor_set_layout_binding_depot)
    :_vk_descriptor_set_layout(descriptor_set_layout)
    ,_descriptor_set_layout_binding_depot(descriptor_set_layout_binding_depot) {
  }

  const VkDescriptorSetLayout _vk_descriptor_set_layout;
  DescriptorSetLayoutBindingDepot& _descriptor_set_layout_binding_depot;
protected:
private:
};

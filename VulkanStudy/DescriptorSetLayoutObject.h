#pragma once

#include <vulkan/vulkan.h>

class DescriptorSetLayoutObject {
public:
  DescriptorSetLayoutObject(VkDescriptorSetLayout descriptor_set_layout) :_vk_descriptor_set_layout(descriptor_set_layout) {
  }

  const VkDescriptorSetLayout _vk_descriptor_set_layout;
protected:
private:
};

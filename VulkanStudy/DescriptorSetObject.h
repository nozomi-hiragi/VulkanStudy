#pragma once

#include <vulkan/vulkan.h>

class DescriptorSetObject {
public:
  DescriptorSetObject(const VkDescriptorSet descriptor_set) : _vk_descriptor_set(descriptor_set) {
  }
  const VkDescriptorSet _vk_descriptor_set;
protected:
private:
};

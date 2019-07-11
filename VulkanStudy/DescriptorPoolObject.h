#pragma once

#include <vulkan/vulkan.h>

class DescriptorPoolObject {
public:
  DescriptorPoolObject(const VkDescriptorPool descriptor_pool) : _vk_descriptor_pool(descriptor_pool) {
  }
  const VkDescriptorPool _vk_descriptor_pool;
protected:
private:
};

#pragma once

#include <vulkan/vulkan.h>

class DescriptorSetObject {
public:
  DescriptorSetObject(const VkDescriptorSet descriptor_set) : _vk_descriptor_set(descriptor_set) {
  }

  static void vkUpdateDescriptorSets_(VkDevice device, std::vector<VkWriteDescriptorSet> write_descriptor_sets) {
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
  }

  const VkDescriptorSet _vk_descriptor_set;
protected:
private:
};

#pragma once

#include <vulkan/vulkan.h>

class DescriptorSetObject {
public:
  DescriptorSetObject(const VkDescriptorSet descriptor_set) : _vk_descriptor_set(descriptor_set) {
  }

  static void vkUpdateDescriptorSets_(VkDevice device, VkDescriptorSet descriptor_set, VkDescriptorBufferInfo& descriptor_buffer_info) {
    VkWriteDescriptorSet write_descriptor_set = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write_descriptor_set.dstSet = descriptor_set;
    write_descriptor_set.dstBinding = 0;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    write_descriptor_set.pImageInfo = nullptr;
    write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
    write_descriptor_set.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device, 1, &write_descriptor_set, 0, nullptr);
  }

  const VkDescriptorSet _vk_descriptor_set;
protected:
private:
};

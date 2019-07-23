#pragma once

#include <vulkan/vulkan.h>

class DescriptorSetObject {
public:
  DescriptorSetObject(const VkDescriptorSet descriptor_set) : _vk_descriptor_set(descriptor_set) {
  }

  static void vkUpdateDescriptorSets_(VkDevice device, VkDescriptorSet descriptor_set, VkDescriptorBufferInfo& descriptor_buffer_info) {
    VkWriteDescriptorSet write_descriptor_sets[] = {
      {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        nullptr,
        descriptor_set,
        0,
        0,
        1,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        nullptr,
        &descriptor_buffer_info,
        nullptr
      }
    };

    vkUpdateDescriptorSets(device, 1, write_descriptor_sets, 0, nullptr);
  }

  const VkDescriptorSet _vk_descriptor_set;
protected:
private:
};

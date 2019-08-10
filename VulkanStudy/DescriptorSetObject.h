#pragma once

#include <vulkan/vulkan.h>
#include "BufferObject.h"
#include "SamplerObject.h"
#include "ImageViewObject.h"

class DescriptorSetObject {
public:
  DescriptorSetObject(const VkDescriptorSet descriptor_set) : _vk_descriptor_set(descriptor_set) {
  }

  static void vkUpdateDescriptorSets_(VkDevice device, std::vector<VkWriteDescriptorSet> write_descriptor_sets) {
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
  }

  void updateDescriptorSet(VkDevice device, const VkDescriptorSetLayoutBinding& set_layout, const VkDescriptorImageInfo* image, const VkDescriptorBufferInfo* buffer) {
    VkWriteDescriptorSet write_descriptor_set = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write_descriptor_set.dstSet = _vk_descriptor_set;
    write_descriptor_set.dstBinding = set_layout.binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorCount = set_layout.descriptorCount;
    write_descriptor_set.descriptorType = set_layout.descriptorType;
    write_descriptor_set.pImageInfo = image;
    write_descriptor_set.pBufferInfo = buffer;
    write_descriptor_set.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device, 1, &write_descriptor_set, 0, nullptr);
  }

  void updateDescriptorSetSampler(VkDevice device, const VkDescriptorSetLayoutBinding& set_layout, std::shared_ptr<SamplerObject> sampler, std::shared_ptr<ImageViewObject> image_view) {
    VkDescriptorImageInfo descriptor_image = {};
    descriptor_image.sampler = sampler->_vk_sampler;
    descriptor_image.imageView = image_view->_vk_image_view;
    descriptor_image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // サンプラーからとる？

    updateDescriptorSet(device, set_layout, &descriptor_image, nullptr);
  }

  void updateDescriptorSetBuffer(VkDevice device, const VkDescriptorSetLayoutBinding& set_layout, std::shared_ptr<BufferObject> buffer, const uint64_t size) {
    VkDescriptorBufferInfo descriptor_buffer = {};
    descriptor_buffer.buffer = buffer->_vk_buffer;
    descriptor_buffer.offset = 0;
    descriptor_buffer.range = size;

    updateDescriptorSet(device, set_layout, nullptr, &descriptor_buffer);
  }

  const VkDescriptorSet _vk_descriptor_set;
protected:
private:
};

#pragma once

#include <vulkan/vulkan.h>

#include "DeviceObject.h"
#include "BufferObject.h"
#include "SamplerObject.h"
#include "ImageViewObject.h"

class DescriptorSetObject {
public:
  DescriptorSetObject(const VkDescriptorSet descriptor_set, std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings)
    : _vk_descriptor_set(descriptor_set)
    , _vk_descriptor_set_layout_bindings(std::move(descriptor_set_layout_bindings)) {
  }

  static void vkUpdateDescriptorSets_(VkDevice device, std::vector<VkWriteDescriptorSet> write_descriptor_sets) {
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
  }

  void updateDescriptorSet(std::shared_ptr<DeviceObject> device, uint32_t binding, const VkDescriptorImageInfo* image, const VkDescriptorBufferInfo* buffer) {
    auto& layout_binding = _vk_descriptor_set_layout_bindings[binding];

    VkWriteDescriptorSet write_descriptor_set = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write_descriptor_set.dstSet = _vk_descriptor_set;
    write_descriptor_set.dstBinding = layout_binding.binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorCount = layout_binding.descriptorCount;
    write_descriptor_set.descriptorType = layout_binding.descriptorType;
    write_descriptor_set.pImageInfo = image;
    write_descriptor_set.pBufferInfo = buffer;
    write_descriptor_set.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device->_vk_device, 1, &write_descriptor_set, 0, nullptr);
  }

  void updateDescriptorSetSampler(std::shared_ptr<DeviceObject> device, uint32_t binding, std::shared_ptr<SamplerObject> sampler, std::shared_ptr<ImageViewObject> image_view) {
    VkDescriptorImageInfo descriptor_image = {};
    descriptor_image.sampler = sampler->_vk_sampler;
    descriptor_image.imageView = image_view->_vk_image_view;
    descriptor_image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // サンプラーからとる？

    updateDescriptorSet(device, binding, &descriptor_image, nullptr);
  }

  void updateDescriptorSetBuffer(std::shared_ptr<DeviceObject> device, uint32_t binding, std::shared_ptr<BufferObject> buffer, const uint64_t size) {
    VkDescriptorBufferInfo descriptor_buffer = {};
    descriptor_buffer.buffer = buffer->_vk_buffer;
    descriptor_buffer.offset = 0;
    descriptor_buffer.range = size;

    updateDescriptorSet(device, binding, nullptr, &descriptor_buffer);
  }

  const VkDescriptorSet _vk_descriptor_set;
  const std::vector<VkDescriptorSetLayoutBinding> _vk_descriptor_set_layout_bindings;
protected:
private:
};

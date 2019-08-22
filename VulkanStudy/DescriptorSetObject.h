#pragma once

#include <vulkan/vulkan.h>

#include "DeviceObject.h"
#include "BufferObject.h"
#include "SamplerObject.h"
#include "ImageViewObject.h"
#include "DescriptorSetLayoutDepot.h"

class DescriptorSetObject {
public:
  DescriptorSetObject(const VkDescriptorSet descriptor_set, DescriptorSetLayoutBindingDepot& descriptor_set_layout_binding_depot)
    :_vk_descriptor_set(descriptor_set)
    ,_descriptor_set_layout_binding_depot(descriptor_set_layout_binding_depot) {
  }

  static void vkUpdateDescriptorSets_(VkDevice device, std::vector<VkWriteDescriptorSet> write_descriptor_sets) {
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
  }

  void updateDescriptorSet(std::shared_ptr<DeviceObject> device, std::string set_layout_name, const VkDescriptorImageInfo* image, const VkDescriptorBufferInfo* buffer) {
    auto& set_layout = _descriptor_set_layout_binding_depot.get(set_layout_name);

    VkWriteDescriptorSet write_descriptor_set = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write_descriptor_set.dstSet = _vk_descriptor_set;
    write_descriptor_set.dstBinding = set_layout.binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorCount = set_layout.descriptorCount;
    write_descriptor_set.descriptorType = set_layout.descriptorType;
    write_descriptor_set.pImageInfo = image;
    write_descriptor_set.pBufferInfo = buffer;
    write_descriptor_set.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device->_vk_device, 1, &write_descriptor_set, 0, nullptr);
  }

  void updateDescriptorSetSampler(std::shared_ptr<DeviceObject> device, std::string set_layout_name, std::shared_ptr<SamplerObject> sampler, std::shared_ptr<ImageViewObject> image_view) {
    VkDescriptorImageInfo descriptor_image = {};
    descriptor_image.sampler = sampler->_vk_sampler;
    descriptor_image.imageView = image_view->_vk_image_view;
    descriptor_image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // サンプラーからとる？

    updateDescriptorSet(device, set_layout_name, &descriptor_image, nullptr);
  }

  void updateDescriptorSetBuffer(std::shared_ptr<DeviceObject> device, std::string set_layout_name, std::shared_ptr<BufferObject> buffer, const uint64_t size) {
    VkDescriptorBufferInfo descriptor_buffer = {};
    descriptor_buffer.buffer = buffer->_vk_buffer;
    descriptor_buffer.offset = 0;
    descriptor_buffer.range = size;

    updateDescriptorSet(device, set_layout_name, nullptr, &descriptor_buffer);
  }

  const VkDescriptorSet _vk_descriptor_set;
  DescriptorSetLayoutBindingDepot& _descriptor_set_layout_binding_depot;
protected:
private:
};

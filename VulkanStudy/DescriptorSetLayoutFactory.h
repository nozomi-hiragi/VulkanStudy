#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "DescriptorSetLayoutObject.h"

class DescriptorSetLayoutFactory {
  static VkDescriptorSetLayout _createVkDescriptorSetLayout(VkDevice device) {
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {};
    descriptor_set_layout_binding.binding = 0;
    descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptor_set_layout_binding.descriptorCount = 1;
    descriptor_set_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptor_set_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptor_set_layout_info.bindingCount = 1;
    descriptor_set_layout_info.pBindings = &descriptor_set_layout_binding;

    VkDescriptorSetLayout descriptor_set_layout;
    vkCreateDescriptorSetLayout(device, &descriptor_set_layout_info, nullptr, &descriptor_set_layout);
    return descriptor_set_layout;
  }

  static void _destroyVkDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayout descriptor_set_layout) {
    vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
  }

  std::shared_ptr<DescriptorSetLayoutObject> _createCore(VkDevice device) {
    auto vk_descriptoir_set_layout = _createVkDescriptorSetLayout(device);
    return std::make_shared<DescriptorSetLayoutObject>(vk_descriptoir_set_layout);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<DescriptorSetLayoutObject> object) {
    _destroyVkDescriptorSetLayout(device, object->_vk_descriptor_set_layout);
  }

public:
  std::shared_ptr<DescriptorSetLayoutObject> createObject(VkDevice device) {
    auto object = _createCore(device);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<DescriptorSetLayoutObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyCore(device, object);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<DescriptorSetLayoutObject>> _container;
};

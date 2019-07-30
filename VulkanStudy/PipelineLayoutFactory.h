#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "PipelineLayoutObject.h"

class PipelineLayoutFactory {
  static VkPipelineLayout _createVkPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptor_set_layout) {
    VkPipelineLayoutCreateInfo pipeline_layout_info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptor_set_layout;

    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout);
    return pipeline_layout;
  }

  static void _destroyVkPipelineLayout(VkDevice device, const VkPipelineLayout pipeline_layout) {
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
  }

  std::shared_ptr<PipelineLayoutObject> _createCore(VkDevice device, VkDescriptorSetLayout descriptor_set_layout) {
    auto vk_pipeline_layout = _createVkPipelineLayout(device, descriptor_set_layout);
    return std::make_shared<PipelineLayoutObject>(vk_pipeline_layout);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<PipelineLayoutObject> object) {
    _destroyVkPipelineLayout(device, object->_vk_pipeline_layout);
  }

public:
  std::shared_ptr<PipelineLayoutObject> createObject(VkDevice device, VkDescriptorSetLayout descriptor_set_layout) {
    auto object = _createCore(device, descriptor_set_layout);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<PipelineLayoutObject>& object) {
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
  std::set<std::shared_ptr<PipelineLayoutObject>> _container;
};

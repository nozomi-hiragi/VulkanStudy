#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "PipelineLayoutObject.h"
#include "DeviceObject.h"

class PipelineLayoutFactory : public AbstractFactory<PipelineLayoutObject, DeviceObject, const VkPushConstantRange&, VkDescriptorSetLayout > {
public:
protected:
private:
  static VkPipelineLayout _createVkPipelineLayout(VkDevice device, const VkPushConstantRange& range, VkDescriptorSetLayout descriptor_set_layout) {
    VkPipelineLayoutCreateInfo pipeline_layout_info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &range;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptor_set_layout;

    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout);
    return pipeline_layout;
  }

  static void _destroyVkPipelineLayout(VkDevice device, const VkPipelineLayout pipeline_layout) {
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
  }

  std::shared_ptr<PipelineLayoutObject> _createCore(const VkPushConstantRange& range, VkDescriptorSetLayout descriptor_set_layout) {
    auto vk_pipeline_layout = _createVkPipelineLayout(_parent->_vk_device, range, descriptor_set_layout);
    return std::make_shared<PipelineLayoutObject>(vk_pipeline_layout);
  }

  void _destroyCore(std::shared_ptr<PipelineLayoutObject> object) {
    _destroyVkPipelineLayout(_parent->_vk_device, object->_vk_pipeline_layout);
  }

};

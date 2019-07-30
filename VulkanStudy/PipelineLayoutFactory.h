#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "PipelineLayoutObject.h"
#include "DeviceObject.h"

class PipelineLayoutFactory : public AbstractFactory<PipelineLayoutObject, DeviceObject, const std::vector<VkDescriptorSetLayout>& > {
public:
protected:
private:
  static VkPipelineLayout _createVkPipelineLayout(VkDevice device, VkPipelineLayoutCreateInfo& pipeline_layout_info) {
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout);
    return pipeline_layout;
  }

  static void _destroyVkPipelineLayout(VkDevice device, const VkPipelineLayout pipeline_layout) {
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
  }

  std::shared_ptr<PipelineLayoutObject> _createCore(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) {
    VkPipelineLayoutCreateInfo pipeline_layout_info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();

    auto vk_pipeline_layout = _createVkPipelineLayout(_parent->_vk_device, pipeline_layout_info);
    return std::make_shared<PipelineLayoutObject>(vk_pipeline_layout);
  }

  void _destroyCore(std::shared_ptr<PipelineLayoutObject> object) {
    _destroyVkPipelineLayout(_parent->_vk_device, object->_vk_pipeline_layout);
  }

};

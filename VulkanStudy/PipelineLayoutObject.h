#pragma once

#include <vulkan/vulkan.h>

class PipelineLayoutObject {
public:
  PipelineLayoutObject(const VkPipelineLayout pipeline_layout) : _vk_pipeline_layout(pipeline_layout) {
  }
  const VkPipelineLayout _vk_pipeline_layout;
protected:
private:
};

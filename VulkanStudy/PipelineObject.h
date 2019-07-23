#pragma once

#include <vulkan/vulkan.h>

class PipelineObject {
public:
  PipelineObject(const VkPipeline pipeline) : _vk_pipeline(pipeline) {
  }

  const VkPipeline _vk_pipeline;
protected:
private:
};

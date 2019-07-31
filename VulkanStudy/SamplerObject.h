#pragma once

#include <vulkan/vulkan.h>

class SamplerObject {
public:
  SamplerObject(const VkSampler sampler) : _vk_sampler(sampler) {
  }

  const VkSampler _vk_sampler;
protected:
private:
};

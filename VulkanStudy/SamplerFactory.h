#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "SamplerObject.h"
#include "DeviceObject.h"

class SamplerFactory : public AbstractFactory<SamplerObject, DeviceObject, VkFilter, VkFilter, VkSamplerMipmapMode, float, float> {
public:
protected:
private:
  static auto _createVkSampler(VkDevice device, VkSamplerCreateInfo& sampler_info) {

    VkSampler sampler;
    auto result = vkCreateSampler(device, &sampler_info, nullptr, &sampler);
    return sampler;
  }

  static void _destroyVkSampler(VkDevice device, VkSampler sampler) {
    vkDestroySampler(device, sampler, nullptr);
  }

  std::shared_ptr<SamplerObject> _createCore(VkFilter mag_filter, VkFilter min_filter, VkSamplerMipmapMode mipmap_mode, float mip_level, float mip_bias) {
    VkSamplerCreateInfo sampler_info = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    sampler_info.flags = 0;
    sampler_info.magFilter = mag_filter;// VK_FILTER_NEAREST;
    sampler_info.minFilter = min_filter;// VK_FILTER_NEAREST;
    sampler_info.mipmapMode = mipmap_mode;// VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.mipLodBias = 0.f;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = 16.f;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_NEVER;
    sampler_info.minLod = 0.f;
    sampler_info.maxLod = mip_level;
    sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;

    auto vk_sampler = _createVkSampler(_parent->_vk_device, sampler_info);
    return std::make_shared<SamplerObject>(vk_sampler);
  }

  void _destroyCore(std::shared_ptr<SamplerObject> object) {
    _destroyVkSampler(_parent->_vk_device, object->_vk_sampler);
  }

};

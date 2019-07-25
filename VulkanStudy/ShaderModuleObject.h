#pragma once

#include <vulkan/vulkan.h>

class ShaderModuleObject {
public:
  ShaderModuleObject(const VkShaderModule vk_shader_module, const VkShaderStageFlagBits shader_stage, const char* entry) :
    _vk_shader_module(vk_shader_module),
    _vk_shader_stage(shader_stage),
    _entry(entry) {
  }

  ~ShaderModuleObject() {
  }

  const VkShaderModule _vk_shader_module;
  const VkShaderStageFlagBits _vk_shader_stage;
  const char*const _entry;
protected:
private:
};

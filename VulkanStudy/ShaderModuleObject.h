#pragma once

#include <vulkan/vulkan.h>

class ShaderModuleObject {
public:
  ShaderModuleObject(const VkShaderModule vk_shader_module) : _vk_shader_module(vk_shader_module) {
  }

  ~ShaderModuleObject() {
  }

  const VkShaderModule _vk_shader_module;
protected:
private:
};

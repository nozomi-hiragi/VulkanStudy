#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "ShaderModuleObject.h"
#include "DeviceObject.h"

class ShaderModuleFactory : public AbstractFactory<ShaderModuleObject, DeviceObject, const size_t, const uint32_t*, const VkShaderStageFlagBits, const char*const> {
public:
protected:
private:
  static VkShaderModule _createVkShaderModule(VkDevice device, const size_t size, const uint32_t* code) {
    VkShaderModuleCreateInfo shader_module_info = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    shader_module_info.codeSize = size;
    shader_module_info.pCode = code;

    VkShaderModule shader_module;
    auto result = vkCreateShaderModule(device, &shader_module_info, nullptr, &shader_module);
    return shader_module;
  }

  static void _destroyVkShaderModule(VkDevice device, const VkShaderModule shader_module) {
    vkDestroyShaderModule(device, shader_module, nullptr);
  }

  std::shared_ptr<ShaderModuleObject> _createCore(const size_t size, const uint32_t* code, const VkShaderStageFlagBits shader_stage, const char*const entry) {
    auto vk_shader_module = _createVkShaderModule(_parent->_vk_device, size, code);
    return std::make_shared<ShaderModuleObject>(vk_shader_module, shader_stage, entry);
  }

  void _destroyCore(std::shared_ptr<ShaderModuleObject> object) {
    _destroyVkShaderModule(_parent->_vk_device, object->_vk_shader_module);
  }

};

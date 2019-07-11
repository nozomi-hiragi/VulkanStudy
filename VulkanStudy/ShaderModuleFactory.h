#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "ShaderModuleObject.h"

class ShaderModuleFactory {
public:
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

  std::shared_ptr<ShaderModuleObject> _createCore(VkDevice device, const size_t size, const uint32_t* code) {
    auto vk_shader_module = _createVkShaderModule(device, size, code);
    return std::make_shared<ShaderModuleObject>(vk_shader_module);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<ShaderModuleObject> object) {
    _destroyVkShaderModule(device, object->_vk_shader_module);
  }

  auto createObject(VkDevice device, const size_t size, const uint32_t* code) {
    auto object = _createCore(device, size, code);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<ShaderModuleObject>& object) {
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
  std::set<std::shared_ptr<ShaderModuleObject>> _container;
};

#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "DescriptorSetLayoutObject.h"
#include "DeviceObject.h"

#include "DescriptorSetLayoutDepot.h"

class DescriptorSetLayoutFactory : public AbstractFactory<DescriptorSetLayoutObject, DeviceObject, const std::vector<std::string>&> {
public:
  auto& getDescriptorSetLayoutBindingDepot() {
    return _descriptor_set_layout_binding_depot;
  }

protected:
private:
  static VkDescriptorSetLayout _createVkDescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo& descriptor_set_layout_info) {
    VkDescriptorSetLayout descriptor_set_layout;
    vkCreateDescriptorSetLayout(device, &descriptor_set_layout_info, nullptr, &descriptor_set_layout);
    return descriptor_set_layout;
  }

  static void _destroyVkDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayout descriptor_set_layout) {
    vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
  }

  std::shared_ptr<DescriptorSetLayoutObject> _createCore(const std::vector<std::string>& descriptor_set_layout_binding_names) {
    std::vector<VkDescriptorSetLayoutBinding> _descriptor_set_layout_bindings;
    _descriptor_set_layout_bindings.reserve(descriptor_set_layout_binding_names.size());
    for (const auto& it : descriptor_set_layout_binding_names) {
      _descriptor_set_layout_bindings.push_back(_descriptor_set_layout_binding_depot.get(it));
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(_descriptor_set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = _descriptor_set_layout_bindings.data();

    auto vk_descriptoir_set_layout = _createVkDescriptorSetLayout(_parent->_vk_device, descriptor_set_layout_info);
    return std::make_shared<DescriptorSetLayoutObject>(vk_descriptoir_set_layout);
  }

  void _destroyCore(std::shared_ptr<DescriptorSetLayoutObject> object) {
    _destroyVkDescriptorSetLayout(_parent->_vk_device, object->_vk_descriptor_set_layout);
  }

  DescriptorSetLayoutBindingDepot _descriptor_set_layout_binding_depot;
};

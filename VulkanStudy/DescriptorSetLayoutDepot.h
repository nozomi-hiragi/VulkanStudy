#pragma once

#pragma once

#include <vulkan/vulkan.h>

#include "AbstractDepot.h"

class DescriptorSetLayoutBindingDepot : public AbstractDepot<VkDescriptorSetLayoutBinding> {
public:
  void add(const std::string name,
    const uint32_t              binding,
    const VkDescriptorType      descriptor_type,
    const uint32_t              descriptor_count,
    const VkShaderStageFlags    stage_flags,
    const VkSampler*const       immutable_samplers
  ) {

    VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {};
    descriptor_set_layout_binding.binding = binding;
    descriptor_set_layout_binding.descriptorType = descriptor_type;
    descriptor_set_layout_binding.descriptorCount = descriptor_count;
    descriptor_set_layout_binding.stageFlags = stage_flags;
    descriptor_set_layout_binding.pImmutableSamplers = immutable_samplers;

    AbstractDepot::add(name, descriptor_set_layout_binding);
  }

protected:
private:
};

//class VertexInputAttributeDescriptionDepot : public AbstractDepot<VkVertexInputAttributeDescription> {
//public:
//  void add(const std::string name,
//    const uint32_t    location,
//    const uint32_t    binding,
//    const VkFormat    format,
//    const uint32_t    offset) {
//
//    VkVertexInputAttributeDescription vertex_input_attribute_description = {};
//    vertex_input_attribute_description.location = location;
//    vertex_input_attribute_description.binding = binding;
//    vertex_input_attribute_description.format = format;
//    vertex_input_attribute_description.offset = offset;
//
//    AbstractDepot::add(name, vertex_input_attribute_description);
//  }
//
//protected:
//private:
//};

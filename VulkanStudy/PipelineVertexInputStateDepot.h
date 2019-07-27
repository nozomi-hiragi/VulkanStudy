#pragma once

#include <vulkan/vulkan.h>

#include "AbstractDepot.h"

class VertexInputBindingDescriptionDepot : public AbstractDepot<VkVertexInputBindingDescription> {
public:
  void add(const std::string name,
    const uint32_t           binding,
    const uint32_t           stride,
    const VkVertexInputRate  input_rate) {

    VkVertexInputBindingDescription vertex_input_binding_description = {};
    vertex_input_binding_description.binding = binding;
    vertex_input_binding_description.stride = stride;
    vertex_input_binding_description.inputRate = input_rate;

    AbstractDepot::add(name, vertex_input_binding_description);
  }

protected:
private:
};

class VertexInputAttributeDescriptionDepot : public AbstractDepot<VkVertexInputAttributeDescription> {
public:
  void add(const std::string name,
    const uint32_t    location,
    const uint32_t    binding,
    const VkFormat    format,
    const uint32_t    offset) {

    VkVertexInputAttributeDescription vertex_input_attribute_description = {};
    vertex_input_attribute_description.location = location;
    vertex_input_attribute_description.binding = binding;
    vertex_input_attribute_description.format = format;
    vertex_input_attribute_description.offset = offset;

    AbstractDepot::add(name, vertex_input_attribute_description);
  }

protected:
private:
};

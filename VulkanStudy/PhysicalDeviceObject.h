#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "ImageObject.h"
#include "BufferObject.h"

class MemoryProperties {
public:
  MemoryProperties(VkPhysicalDeviceMemoryProperties vk_memory_properties)
    : _vk_memory_properties(vk_memory_properties) {}

  template<class Object>
  const uint32_t findProperties(std::shared_ptr<Object> object, VkMemoryPropertyFlags properties) const {
    auto memory_type_bits = object->_vk_memory_requirements.memoryTypeBits;
    auto& memory_types = _vk_memory_properties.memoryTypes;
    for (uint32_t i = 0; i < _vk_memory_properties.memoryTypeCount; i++) {
      if (memory_type_bits & 1 &&
        (memory_types[i].propertyFlags & properties) == properties) {
        return i;
      }
      memory_type_bits >>= 1;
    }
    return -1;
  }

protected:
private:
  const VkPhysicalDeviceMemoryProperties _vk_memory_properties;
};

class PhysicalDeviceObject {
public:
  PhysicalDeviceObject(
    const VkPhysicalDevice physical_device,
    const VkPhysicalDeviceFeatures physical_device_features,
    const VkPhysicalDeviceMemoryProperties memory_properties,
    const std::vector<VkQueueFamilyProperties> queue_family_properties) :
    _vk_physical_device(physical_device),
    _vk_physical_device_features(physical_device_features),
    _memory_properties(memory_properties),
    _vk_queue_family_properties(queue_family_properties) {
  }

  ~PhysicalDeviceObject() {
  }

  const VkPhysicalDevice _vk_physical_device;
  const VkPhysicalDeviceFeatures _vk_physical_device_features;
  const MemoryProperties _memory_properties;
  const std::vector<VkQueueFamilyProperties> _vk_queue_family_properties;
protected:
private:
};

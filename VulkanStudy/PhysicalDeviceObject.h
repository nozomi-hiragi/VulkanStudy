#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class PhysicalDeviceObject {
public:
  PhysicalDeviceObject(
    const VkPhysicalDevice physical_device,
    const VkPhysicalDeviceMemoryProperties memory_properties,
    const std::vector<VkQueueFamilyProperties> queue_family_properties) :
    _physical_device(physical_device),
    _memory_properties(memory_properties),
    _queue_family_properties(queue_family_properties) {
  }

  ~PhysicalDeviceObject() {
  }

  const uint32_t findProperties(uint32_t memory_type_bits, VkMemoryPropertyFlags properties) {
    auto& memory_types = _memory_properties.memoryTypes;
    for (uint32_t i = 0; i < _memory_properties.memoryTypeCount; i++) {
      if (memory_type_bits & 1 &&
        (memory_types[i].propertyFlags & properties) == properties) {
          return i;
      }
      memory_type_bits >>= 1;
    }
    return -1;
  }

  const VkPhysicalDevice _physical_device;
  const VkPhysicalDeviceMemoryProperties _memory_properties;
  const std::vector<VkQueueFamilyProperties> _queue_family_properties;
protected:
private:
};

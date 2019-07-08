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

  const VkPhysicalDevice _physical_device;
  const VkPhysicalDeviceMemoryProperties _memory_properties;
  const std::vector<VkQueueFamilyProperties> _queue_family_properties;
protected:
private:
};

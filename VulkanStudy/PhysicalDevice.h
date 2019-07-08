#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <memory>

#include "InstanceObject.h"

class PhysicalDeviceObject {
public:
  PhysicalDeviceObject(
    const VkPhysicalDevice physical_device,
    const VkPhysicalDeviceMemoryProperties memory_properties,
    const std::vector<VkQueueFamilyProperties> queue_family_properties):
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

class PhysicalDeviceFactory {
public:
  PhysicalDeviceFactory() {
  }

  ~PhysicalDeviceFactory() {
  }

  std::shared_ptr<PhysicalDeviceObject> createPhysicalDevice(std::shared_ptr<InstanceObject> instance, uint32_t index);

  void destroyPhysicalDevice(std::shared_ptr<PhysicalDeviceObject>& object);

protected:
private:
  std::set<std::shared_ptr<PhysicalDeviceObject>> _container;
};

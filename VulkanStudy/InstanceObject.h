#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "PhysicalDeviceObject.h"

class InstanceObject {
public:
  InstanceObject(const VkInstance instance, const std::vector<std::shared_ptr<PhysicalDeviceObject>> physical_devices):
    _vk_instance(instance),
    _physical_devices(physical_devices) {
  }

  ~InstanceObject() {
  }

  const VkInstance _vk_instance;
  const std::vector<std::shared_ptr<PhysicalDeviceObject>> _physical_devices;
protected:
private:
};

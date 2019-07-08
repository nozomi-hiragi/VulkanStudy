#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class InstanceObject {
public:
  InstanceObject(const VkInstance instance, const std::vector<VkPhysicalDevice> devices) :
    _vk_instance(instance),
    _devices(devices) {
  }

  ~InstanceObject() {
  }

  const VkInstance _vk_instance;
  const std::vector<VkPhysicalDevice> _devices;
protected:
private:
};

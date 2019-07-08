#pragma once

#include <vulkan/vulkan.h>
#include <set>
#include <memory>

#include "PhysicalDeviceObject.h"
#include "InstanceObject.h"

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

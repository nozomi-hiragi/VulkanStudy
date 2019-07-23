#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>

#include "PhysicalDeviceObject.h"
#include "QueueObject.h"

class Device {
public:
  static Device createDevice(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface);

  Device(): _device(nullptr) {
  }

  Device(const vk::Device device) :
    _device(device) {
  }

  ~Device() {
  }

  void destroy() {
    if (!_device) { return; }
    _device.waitIdle();
    _device.destroy();
    _device = nullptr;
  }

  // for queue
  auto getQueue() {
    return _queue;
  }

  auto getVkDevice() {
    return _device;
  }

protected:
private:
  vk::Device _device;
  std::shared_ptr<QueueObject> _queue;
};

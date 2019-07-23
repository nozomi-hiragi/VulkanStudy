#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "QueueObject.h"

class DeviceObject {
public:
  DeviceObject(const VkDevice device, const std::shared_ptr<QueueObject> queue) :
    _vk_device(device),
    _queue(queue) {
  }

  const VkDevice _vk_device;
  const std::shared_ptr<QueueObject> _queue;
protected:
private:
};

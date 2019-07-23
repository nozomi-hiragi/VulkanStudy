#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "QueueObject.h"

class DeviceObject {
public:
  DeviceObject(const VkDevice device, const std::shared_ptr<QueueObject> queue_object) :
    _vk_device(device),
    _queue_object(queue_object) {
  }

  const VkDevice _vk_device;
  const std::shared_ptr<QueueObject> _queue_object;
protected:
private:
};

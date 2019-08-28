#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "StandardFactory.h"
#include "SemaphoreObject.h"
#include "DeviceObject.h"

class SemaphoreFactory : public StandardFactory<SemaphoreObject, DeviceObject> {
public:
protected:
private:
  static auto _createVkSemaphore(VkDevice device) {
    VkSemaphoreCreateInfo semaphore_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    VkSemaphore semaphore;
    vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore);
    return semaphore;
  }

  static void _destroyVkSemaphore(VkDevice device, VkSemaphore semaphore) {
    vkDestroySemaphore(device, semaphore, nullptr);
  }

  std::shared_ptr<SemaphoreObject> _createCore() {
    auto vk_semaphore = _createVkSemaphore(_parent->_vk_device);
    return std::make_shared<SemaphoreObject>(vk_semaphore);
  }

  void _destroyCore(std::shared_ptr<SemaphoreObject> object) {
    _destroyVkSemaphore(_parent->_vk_device, object->_vk_semaphore);
  }

};

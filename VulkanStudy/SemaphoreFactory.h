#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "SemaphoreObject.h"

class SemaphoreFactory {
  static auto _createVkSemaphore(VkDevice device) {
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore);
    return semaphore;
  }

  static void _destroyVkSemaphore(VkDevice device, VkSemaphore semaphore) {
    vkDestroySemaphore(device, semaphore, nullptr);
  }

public:
  SemaphoreFactory() {
  }

  ~SemaphoreFactory() {
  }

  auto createSemaphore(VkDevice device) {
    auto vk_semaphore = _createVkSemaphore(device);
    auto object = std::make_shared<SemaphoreObject>(vk_semaphore);
    _container.insert(object);
    return object;
  }

  void destroySemaphore(VkDevice device, std::shared_ptr<SemaphoreObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyVkSemaphore(device, object->_vk_semaphore);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<SemaphoreObject>> _container;
};

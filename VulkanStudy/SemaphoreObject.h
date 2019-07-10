#pragma once

#include <vulkan/vulkan.h>

class SemaphoreObject {
public:
  SemaphoreObject(const VkSemaphore semaphore) : _vk_semaphore(semaphore) {
  }

  ~SemaphoreObject() {
  }

  const VkSemaphore _vk_semaphore;
protected:
private:
};

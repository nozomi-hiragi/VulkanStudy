#pragma once

#include <vulkan/vulkan.h>

class QueueObject {
public:
  QueueObject(const VkQueue queue): _vk_queue(queue) {
  }

  ~QueueObject() {
  }

  const VkQueue _vk_queue;
protected:
private:
};

#pragma once

#include <vulkan/vulkan.h>

class QueueObject {
public:
  QueueObject(const VkQueue queue, const uint32_t queue_family_index):
    _vk_queue(queue),
    _vk_queue_family_index(queue_family_index){
  }

  ~QueueObject() {
  }

  const VkQueue _vk_queue;
  const uint32_t _vk_queue_family_index;
protected:
private:
};

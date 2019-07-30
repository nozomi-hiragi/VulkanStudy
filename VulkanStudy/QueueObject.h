#pragma once

#include <vulkan/vulkan.h>

#include "FenceObject.h"

class QueueObject {
public:
  QueueObject(const VkQueue queue, const uint32_t queue_family_index):
    _vk_queue(queue),
    _vk_queue_family_index(queue_family_index){
  }

  ~QueueObject() {
  }

  void submit(const uint32_t count, const VkSubmitInfo* submits, const std::shared_ptr<FenceObject> fence) {
    vkQueueSubmit(_vk_queue, count, submits, fence->_vk_fence);
  }

  void present(const VkPresentInfoKHR& present_info) {
    vkQueuePresentKHR(_vk_queue, &present_info);
  }

  const VkQueue _vk_queue;
  const uint32_t _vk_queue_family_index;
protected:
private:
};

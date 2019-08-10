#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

#include "FenceObject.h"
#include "QueueDepot.h"

class QueueObject {
public:
  QueueObject(const VkQueue queue, const uint32_t queue_family_index):
    _vk_queue(queue),
    _vk_queue_family_index(queue_family_index){
  }

  ~QueueObject() {
  }

  void registSubmitInfoName(uint32_t index, std::vector<std::string> names) {
    if (_scheduled_vk_submit_infos.size() <= index) {
      _scheduled_vk_submit_infos.resize(index + 1);
    }

    for (const auto& it : names) {
      const auto& submit_info = _submit_depot.get(it);
      _scheduled_vk_submit_infos[index].push_back(submit_info._vk_submit_info);
    }
  }

  void submit(const uint32_t index, const std::shared_ptr<FenceObject> fence) {
    VkFence vk_fence = fence ? fence->_vk_fence : nullptr;
    const auto& vk_submit_infos = _scheduled_vk_submit_infos[index];
    vkQueueSubmit(_vk_queue, static_cast<uint32_t>(vk_submit_infos.size()), vk_submit_infos.data(), vk_fence);
  }

  void present(const VkPresentInfoKHR& present_info) {
    vkQueuePresentKHR(_vk_queue, &present_info);
  }

  void waitIdle() {
    vkQueueWaitIdle(_vk_queue);
  }

  std::vector<std::vector<VkSubmitInfo>> _scheduled_vk_submit_infos;

  const VkQueue _vk_queue;
  const uint32_t _vk_queue_family_index;
  SubmitDepot _submit_depot;
protected:
private:
};

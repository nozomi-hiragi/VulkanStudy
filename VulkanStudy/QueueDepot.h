#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "AbstractDepot.h"
#include "SemaphoreObject.h"
#include "CommandBufferObject.h"

struct SubmitInfo {
  VkSubmitInfo _vk_submit_info;
  std::shared_ptr<VkSemaphore[]> _wait_semaphores_ptr;
  std::shared_ptr<VkPipelineStageFlags> _wait_dst_stage_mask_ptr;
  std::shared_ptr<VkCommandBuffer[]> _command_buffers_ptr;
  std::shared_ptr<VkSemaphore[]> _signal_semaphores_ptr;
};

class SubmitDepot : public AbstractDepot<SubmitInfo> {
public:
  void add(const std::string name,
    const std::vector<std::shared_ptr<SemaphoreObject>> wait_semaphores,
    const VkPipelineStageFlags*const   wait_dst_stage_mask,
    const std::vector<std::shared_ptr<CommandBufferObject>> command_buffers,
    const std::vector<std::shared_ptr<SemaphoreObject>> signal_semaphores
  ) {
    SubmitInfo submit_info = {};
    submit_info._vk_submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

    if (wait_dst_stage_mask != nullptr) {
      submit_info._wait_dst_stage_mask_ptr = std::make_shared<VkPipelineStageFlags>(*wait_dst_stage_mask);
      submit_info._vk_submit_info.pWaitDstStageMask = submit_info._wait_dst_stage_mask_ptr.get();
    }

    submit_info._wait_semaphores_ptr = std::shared_ptr<VkSemaphore[]>{ new VkSemaphore[wait_semaphores.size()] };
    for (int i = 0; i < wait_semaphores.size(); i++) {
      submit_info._wait_semaphores_ptr[i] = wait_semaphores[i]->_vk_semaphore;
    }

    submit_info._command_buffers_ptr = std::shared_ptr<VkCommandBuffer[]>{ new VkCommandBuffer[command_buffers.size()] };
    for (int i = 0; i < command_buffers.size(); i++) {
      submit_info._command_buffers_ptr[i] = command_buffers[i]->_vk_command_buffer;
    }

    submit_info._signal_semaphores_ptr = std::shared_ptr<VkSemaphore[]>{ new VkSemaphore[signal_semaphores.size()] };
    for (int i = 0; i < signal_semaphores.size(); i++) {
      submit_info._signal_semaphores_ptr[i] = signal_semaphores[i]->_vk_semaphore;
    }

    submit_info._vk_submit_info.waitSemaphoreCount   = static_cast<uint32_t>(wait_semaphores.size());
    submit_info._vk_submit_info.pWaitSemaphores      = submit_info._wait_semaphores_ptr.get();
    submit_info._vk_submit_info.commandBufferCount   = static_cast<uint32_t>(command_buffers.size());
    submit_info._vk_submit_info.pCommandBuffers      = submit_info._command_buffers_ptr.get();
    submit_info._vk_submit_info.signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size());
    submit_info._vk_submit_info.pSignalSemaphores    = submit_info._signal_semaphores_ptr.get();

    AbstractDepot::add(name, submit_info);
  }

protected:
private:
};

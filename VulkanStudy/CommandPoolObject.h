#pragma once

#include <vulkan/vulkan.h>
#include "CommandBuffer.h"

class CommandPoolObject {
public:
  CommandPoolObject(const VkCommandPool command_pool) : _vk_command_pool(command_pool) {
  }

  ~CommandPoolObject() {
  }

  CommandBuffer allocateCommandBuffer(VkDevice device) {
    VkCommandBufferAllocateInfo command_buffer_info = {};
    command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_info.commandPool = _vk_command_pool;
    command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_info.commandBufferCount = 1;

    VkCommandBuffer command_buffers[1];
    vkAllocateCommandBuffers(device, &command_buffer_info, command_buffers);

    return CommandBuffer(command_buffers[0]);
  }

  void freeCommandBuffers(VkDevice device, CommandBuffer command_buffer) {
    VkCommandBuffer vk_command_buffer = command_buffer.getVkCommandBuffer();
    if (!vk_command_buffer) { return; }
    vkFreeCommandBuffers(device, _vk_command_pool, 1, &vk_command_buffer);
    command_buffer.setVkCommandBuffer(nullptr);
  }

  const VkCommandPool _vk_command_pool;
protected:
private:
};

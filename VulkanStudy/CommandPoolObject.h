#pragma once

#include <vulkan/vulkan.h>

class CommandPoolObject {
public:
  CommandPoolObject(const VkCommandPool command_pool) : _vk_command_pool(command_pool) {
  }

  ~CommandPoolObject() {
  }

  const VkCommandPool _vk_command_pool;
protected:
private:
};

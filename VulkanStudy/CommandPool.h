#pragma once

#include <vulkan/vulkan.hpp>

class CommandPool {
public:
  CommandPool(): _command_pool(nullptr) {
  }

  CommandPool(const vk::CommandPool command_pool) : _command_pool(command_pool) {
  }

  ~CommandPool() {
  }

  void setVkCommandPool(const vk::CommandPool command_pool) {
    _command_pool = command_pool;
  }

  vk::CommandPool getVkCommandPool() {
    return _command_pool;
  }

protected:
private:
  vk::CommandPool _command_pool;
};

#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "CommandPoolObject.h"

class CommandPoolFactory {
  static auto _createVkCommandPool(VkDevice device, uint32_t queue_family_index) {
    VkCommandPoolCreateInfo command_pool_info = {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.queueFamilyIndex = queue_family_index;
    command_pool_info.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkCommandPool command_pool;
    auto result = vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool);
    return command_pool;
  }

  static void _destroyVkCommandPool(VkDevice device, VkCommandPool command_pool) {
    vkDestroyCommandPool(device, command_pool, nullptr);
  }

public:
  auto createCommandPool(VkDevice device, uint32_t queue_family_index) {
    auto vk_command_pool = _createVkCommandPool(device, queue_family_index);
    auto object = std::make_shared<CommandPoolObject>(vk_command_pool);
    _container.insert(object);
    return object;
  }

  void destroyCommandPool(VkDevice device, std::shared_ptr<CommandPoolObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyVkCommandPool(device, object->_vk_command_pool);
      object.reset();
    }
  }

  CommandPoolFactory() {
  }

  ~CommandPoolFactory() {
  }

protected:
private:
  std::set<std::shared_ptr<CommandPoolObject>> _container;
};

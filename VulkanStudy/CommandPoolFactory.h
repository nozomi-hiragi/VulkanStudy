#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "CommandPoolObject.h"
#include "DeviceObject.h"
#include "QueueObject.h"

class CommandPoolFactory : public AbstractFactory<CommandPoolObject, DeviceObject, const std::shared_ptr<QueueObject>> {
  static auto _createVkCommandPool(VkDevice device, uint32_t queue_family_index) {
    VkCommandPoolCreateInfo command_pool_info = {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.queueFamilyIndex = queue_family_index;
    command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkCommandPool command_pool;
    auto result = vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool);
    return command_pool;
  }

  static void _destroyVkCommandPool(VkDevice device, VkCommandPool command_pool) {
    vkDestroyCommandPool(device, command_pool, nullptr);
  }

public:
  std::shared_ptr<CommandPoolObject> _createCore(std::shared_ptr<QueueObject> queue) {
    auto vk_command_pool = _createVkCommandPool(_parent->_vk_device, queue->_vk_queue_family_index);
    return std::make_shared<CommandPoolObject>(vk_command_pool);
  }

  void _destroyCore(std::shared_ptr<CommandPoolObject> object) {
    _destroyVkCommandPool(_parent->_vk_device, object->_vk_command_pool);
  }

  CommandPoolFactory() {
  }

  ~CommandPoolFactory() {
  }

protected:
private:
  std::set<std::shared_ptr<CommandPoolObject>> _container;
};

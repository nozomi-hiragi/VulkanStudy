#pragma once

#include <vulkan/vulkan.h>

#include "AbstractFactory.h"
#include "CommandBufferObject.h"
#include "DeviceObject.h"

class CommandPoolObject : public AbstractFactory<CommandBufferObject, DeviceObject> {

  static auto _allocateVkCommandBuffer(VkDevice device, VkCommandPool command_pool) {
    VkCommandBufferAllocateInfo command_buffer_info = {};
    command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_info.commandPool = command_pool;
    command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_info.commandBufferCount = 1;

    VkCommandBuffer command_buffers[1];
    vkAllocateCommandBuffers(device, &command_buffer_info, command_buffers);
    return command_buffers[0];
  }

  static void _freeVkCommandBuffer(VkDevice device, VkCommandPool command_pool, VkCommandBuffer command_buffer) {
    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
  }

  std::shared_ptr<CommandBufferObject> _createCore() {
    auto vk_command_buffer = _allocateVkCommandBuffer(_parent->_vk_device, _vk_command_pool);
    return std::make_shared<CommandBufferObject>(vk_command_buffer);
  }

  void _destroyCore(std::shared_ptr<CommandBufferObject> object) {
    _freeVkCommandBuffer(_parent->_vk_device, _vk_command_pool, object->_vk_command_buffer);
  }

public:
  CommandPoolObject(const VkCommandPool command_pool) : _vk_command_pool(command_pool) {
  }

  ~CommandPoolObject() {
  }

  //std::shared_ptr<CommandBufferObject> allocateCommandBuffer(VkDevice device) {
  //  auto object = _createCore(device);
  //  _container.insert(object);
  //  return object;
  //}

  //void freeCommandBuffers(VkDevice device, std::shared_ptr<CommandBufferObject>& object) {
  //  if (!object) { return; }
  //  auto before = _container.size();
  //  _container.erase(object);
  //  auto after = _container.size();

  //  if (before != after) {
  //    _destroyCore(device, object);
  //    object.reset();
  //  }
  //}

  const VkCommandPool _vk_command_pool;
protected:
private:
  //std::set<std::shared_ptr<CommandBufferObject>> _container;
};

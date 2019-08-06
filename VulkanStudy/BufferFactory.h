#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>
#include <queue>

#include "AbstractFactory.h"
#include "BufferObject.h"
#include "DeviceObject.h"

class BufferFactory : public AbstractFactory<BufferObject, DeviceObject, const VkDeviceSize, const VkBufferUsageFlags, const VkSharingMode> {
public:
  BufferFactory() {
  }

  ~BufferFactory() {
  }

  void executeDestroy() {
    while (!_destroy_queue.empty()) {
      _destroyVkBuffer(_parent->_vk_device, _destroy_queue.front()->_vk_buffer);
      _destroy_queue.pop();
    }
  }

protected:
private:
  static auto _createVkBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharing_mode) {
    VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = sharing_mode;

    VkBuffer buffer;
    auto result = vkCreateBuffer(device, &buffer_info, nullptr, &buffer);
    return buffer;
  }

  static void _destroyVkBuffer(VkDevice device, VkBuffer buffer) {
    vkDestroyBuffer(device, buffer, nullptr);
  }

  static auto _getVkBufferMemoryRequirements(VkDevice device, VkBuffer buffer) {
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);
    return std::move(memory_requirements);
  }

  std::shared_ptr<BufferObject> _createCore(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkSharingMode sharing_mode) {
    auto vk_buffer = _createVkBuffer(_parent->_vk_device, size, usage, sharing_mode);
    auto memory_requirements = _getVkBufferMemoryRequirements(_parent->_vk_device, vk_buffer);
    return std::make_shared<BufferObject>(vk_buffer, std::move(memory_requirements));
  }

  void _destroyCore(std::shared_ptr<BufferObject> object) {
    _destroy_queue.push(object);
  }

  std::queue<std::shared_ptr<BufferObject>> _destroy_queue;
};

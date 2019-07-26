#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "BufferObject.h"
#include "DeviceObject.h"

class BufferFactory : public AbstractFactory<BufferObject, DeviceObject, const VkDeviceSize, const VkBufferUsageFlags > {
public:
  BufferFactory() {
  }

  ~BufferFactory() {
  }

protected:
private:
  static auto _createVkBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage) {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

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

  std::shared_ptr<BufferObject> _createCore(const VkDeviceSize size, const VkBufferUsageFlags usage) {
    auto vk_buffer = _createVkBuffer(_parent->_vk_device, size, usage);
    auto memory_requirements = _getVkBufferMemoryRequirements(_parent->_vk_device, vk_buffer);
    return std::make_shared<BufferObject>(vk_buffer, std::move(memory_requirements));
  }

  void _destroyCore(std::shared_ptr<BufferObject> object) {
    _destroyVkBuffer(_parent->_vk_device, object->_vk_buffer);
  }

};

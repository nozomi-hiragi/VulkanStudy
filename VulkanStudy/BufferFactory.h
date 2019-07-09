#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "BufferObject.h"

class BufferFactory {
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

public:
  BufferFactory() {
  }

  ~BufferFactory() {
  }

  auto createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage) {
    auto vk_buffer = _createVkBuffer(device, size, usage);

    auto object = std::make_shared<BufferObject>(vk_buffer);
    _container.insert(object);
    return object;
  }

  void destroyBuffer(VkDevice device, std::shared_ptr<BufferObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyVkBuffer(device, object->_vk_buffer);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<BufferObject>> _container;
};
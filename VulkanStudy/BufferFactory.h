#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>
#include <queue>

#include "MailingFactory.h"
#include "BufferObject.h"
#include "DeviceObject.h"

struct BufferParams {
  BufferParams() {}
  BufferParams(std::shared_ptr<DeviceObject> device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharing_mode)
  : device(device), size(size), usage(usage), sharing_mode(sharing_mode) {}
  std::shared_ptr<DeviceObject> device;
  VkDeviceSize size;
  VkBufferUsageFlags usage;
  VkSharingMode sharing_mode;
};

using BufferOrder = Order<BufferObject, BufferParams>;
using BufferBorrowed = Borrowed<BufferObject>;

class BufferFactory : public MailingFactory<BufferObject, BufferParams> {
public:
  void executeDestroy(std::shared_ptr<DeviceObject> device) {
    while (!_destroy_queue.empty()) {
      _destroyVkBuffer(device->_vk_device, _destroy_queue.front());
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

  std::shared_ptr<BufferObject> _createObject(BufferParams& params) {
    auto vk_buffer = _createVkBuffer(params.device->_vk_device, params.size, params.usage, params.sharing_mode);
    auto memory_requirements = _getVkBufferMemoryRequirements(params.device->_vk_device, vk_buffer);
    return std::make_shared<BufferObject>(vk_buffer, std::move(memory_requirements));
  }

  void _returnObject(BufferObject* object) {
    _destroy_queue.push(object->_vk_buffer);
  }

  std::queue<VkBuffer> _destroy_queue;
};

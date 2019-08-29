#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "StandardFactory.h"
#include "DeviceMemoryObject.h"
#include "DeviceObject.h"

struct DeviceMemoryParams {
  DeviceMemoryParams() {}
  DeviceMemoryParams(std::shared_ptr<DeviceObject> device, const VkDeviceSize size, const uint32_t type) : device(device), size(size), type(type) {}
  std::shared_ptr<DeviceObject> device;
  VkDeviceSize size;
  uint32_t type;
};

using DeviceMemoryOrder = Order<DeviceMemoryObject, DeviceMemoryParams>;
using DeviceMemoryBorrowed = std::unique_ptr<Borrowed<DeviceMemoryObject>>;

class DeviceMemoryFactory : public MailingFactory<DeviceMemoryObject, DeviceMemoryParams> {
public:
  void executeDestroy(std::shared_ptr<DeviceObject> device) {
    while (!_destroy_queue.empty()) {
      _destroyVkDeviceMemory(device->_vk_device, _destroy_queue.front());
      _destroy_queue.pop();
    }
  }

protected:
private:
  static auto _createVkDeviceMemory(VkDevice device, VkDeviceSize size, uint32_t type) {
    VkMemoryAllocateInfo device_memory_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    device_memory_info.allocationSize = size;
    device_memory_info.memoryTypeIndex = type;

    VkDeviceMemory device_memory;
    auto result = vkAllocateMemory(device, &device_memory_info, nullptr, &device_memory);
    return device_memory;
  }

  static void _destroyVkDeviceMemory(VkDevice device, VkDeviceMemory device_memory) {
    vkFreeMemory(device, device_memory, nullptr);
  }

  std::shared_ptr<DeviceMemoryObject> _createObject(const DeviceMemoryParams& params) {
    auto vk_device_memory = _createVkDeviceMemory(params.device->_vk_device, params.size, params.type);
    return std::make_shared<DeviceMemoryObject>(vk_device_memory);
  }

  void _returnObject(DeviceMemoryObject* object) {
    _destroy_queue.push(object->_vk_device_memory);
  }

  std::queue<VkDeviceMemory> _destroy_queue;
};

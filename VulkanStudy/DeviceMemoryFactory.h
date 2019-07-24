#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "DeviceMemoryObject.h"
#include "DeviceObject.h"

class DeviceMemoryFactory : public AbstractFactory<DeviceMemoryObject, DeviceObject, const VkDeviceSize, const uint32_t> {
public:
  DeviceMemoryFactory() {
  }

  ~DeviceMemoryFactory() {
  }

protected:
private:
  static auto _createVkDeviceMemory(VkDevice device, VkDeviceSize size, uint32_t type) {
    VkMemoryAllocateInfo device_memory_info = {};
    device_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    device_memory_info.allocationSize = size;
    device_memory_info.memoryTypeIndex = type;

    VkDeviceMemory device_memory;
    auto result = vkAllocateMemory(device, &device_memory_info, nullptr, &device_memory);
    return device_memory;
  }

  static void _destroyVkDeviceMemory(VkDevice device, VkDeviceMemory device_memory) {
    vkFreeMemory(device, device_memory, nullptr);
  }

  std::shared_ptr<DeviceMemoryObject> _createCore(VkDeviceSize size, uint32_t type) {
    auto vk_device_memory = _createVkDeviceMemory(_parent->_vk_device, size, type);
    return std::make_shared<DeviceMemoryObject>(vk_device_memory);
  }

  void _destroyCore(std::shared_ptr<DeviceMemoryObject> object) {
    _destroyVkDeviceMemory(_parent->_vk_device, object->_vk_device_memory);
  }

};

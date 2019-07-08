#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "DeviceMemoryObject.h"

class DeviceMemoryFactory {
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

public:
  DeviceMemoryFactory() {
  }

  ~DeviceMemoryFactory() {
  }

  auto createDeviceMemory(VkDevice device, VkDeviceSize size, uint32_t type) {
    auto vk_device_memory = _createVkDeviceMemory(device, size, type);
    auto object = std::make_shared<DeviceMemoryObject>(vk_device_memory);
    _container.insert(object);
    return object;
  }

  void destroyDeviceMemory(VkDevice device, std::shared_ptr<DeviceMemoryObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyVkDeviceMemory(device, object->_vk_device_memory);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<DeviceMemoryObject>> _container;
};

#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>
#include <set>

#include "DeviceObject.h"
#include "PhysicalDeviceObject.h"
#include "QueueObject.h"

class DeviceFactory {
  static uint32_t _findPresentQueueFamilyIndex(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface) {
    std::vector<bool> support_surfaces(physical_device->_queue_family_properties.size());
    for (int i = 0; i < support_surfaces.size(); i++) {
      VkBool32 result;
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_device->_physical_device, i, surface, &result);
      support_surfaces[i] = result == VK_TRUE;
    }

    auto present_queue_family_index = UINT32_MAX;
    for (uint32_t i = 0; i < physical_device->_queue_family_properties.size(); i++) {
      auto is_graphycs_queue_family = (physical_device->_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT;
      auto is_support_surface = support_surfaces[i];
      if (is_graphycs_queue_family && is_support_surface) {
        present_queue_family_index = i;
        break;
      }
    }
    // å©Ç¬Ç©ÇÁÇ»Ç©Ç¡ÇΩÇÁèàóùÇÇ¢ÇÍÇÈÅH

    return present_queue_family_index;
  }

  static VkDevice _createVkDevice(VkPhysicalDevice physical_device, uint32_t present_queue_family_index);

  static void _destroyVkDevice(VkDevice device) {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
  }

  std::shared_ptr<DeviceObject> _createCore(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface) {
    auto present_queue_family_index = _findPresentQueueFamilyIndex(physical_device, surface);
    auto vk_device = _createVkDevice(physical_device->_physical_device, present_queue_family_index);

    VkQueue vk_queue;
    vkGetDeviceQueue(vk_device, present_queue_family_index, 0, &vk_queue);
    auto queue = std::make_shared<QueueObject>(vk_queue, present_queue_family_index);

    return std::make_shared<DeviceObject>(vk_device, queue);
  }

  void _destroyCore(std::shared_ptr<DeviceObject> object) {
    _destroyVkDevice(object->_vk_device);
  }

public:
  std::shared_ptr<DeviceObject> createObject(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface) {
    auto object = _createCore(physical_device, surface);
    _container.insert(object);
    return object;
  }

  void destroyObject(std::shared_ptr<DeviceObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyCore(object);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<DeviceObject>> _container;
};

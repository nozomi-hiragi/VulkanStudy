#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>
#include <set>

#include "StandardFactory.h"
#include "DeviceObject.h"
#include "PhysicalDeviceObject.h"
#include "SurfaceObject.h"
#include "QueueObject.h"

class DeviceFactory : public StandardFactory<DeviceObject, void, const std::shared_ptr<PhysicalDeviceObject>, const std::shared_ptr<SurfaceObject>, const std::vector<const char*>&> {
public:
  auto createObject(const std::shared_ptr<void> parent, const std::shared_ptr<PhysicalDeviceObject> physical_device, const std::shared_ptr<SurfaceObject> surface, const std::vector<const char*>& extensions = std::vector<const char*>()) {
    return StandardFactory::createObject(parent, physical_device, surface, extensions);
  }

protected:
private:
  static uint32_t _findPresentQueueFamilyIndex(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface) {
    std::vector<bool> support_surfaces(physical_device->_vk_queue_family_properties.size());

    if (surface) {
      for (uint32_t i = 0; i < support_surfaces.size(); i++) {
        VkBool32 result;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device->_vk_physical_device, i, surface, &result);
        support_surfaces[i] = result == VK_TRUE;
      }
    } else {
      for (uint32_t i = 0; i < support_surfaces.size(); i++) {
        support_surfaces[i] = true;
      }
    }

    auto present_queue_family_index = UINT32_MAX;
    for (uint32_t i = 0; i < physical_device->_vk_queue_family_properties.size(); i++) {
      auto is_graphycs_queue_family = (physical_device->_vk_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT;
      auto is_support_surface = support_surfaces[i];
      if (is_graphycs_queue_family && is_support_surface) {
        present_queue_family_index = i;
        break;
      }
    }
    // 見つからなかったら処理をいれる？

    return present_queue_family_index;
  }

  static VkDevice _createVkDevice(const std::shared_ptr<PhysicalDeviceObject> physical_device, uint32_t present_queue_family_index, const std::vector<const char*>& extensions);

  static void _destroyVkDevice(VkDevice device) {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
  }

  std::shared_ptr<DeviceObject> _createCore(const std::shared_ptr<PhysicalDeviceObject> physical_device, const std::shared_ptr<SurfaceObject> surface, const std::vector<const char*>& extensions) {
    auto present_queue_family_index = _findPresentQueueFamilyIndex(physical_device, surface ? surface->_vk_surface : nullptr);
    auto vk_device = _createVkDevice(physical_device, present_queue_family_index, extensions);

    VkQueue vk_queue;
    vkGetDeviceQueue(vk_device, present_queue_family_index, 0, &vk_queue);
    auto queue = std::make_shared<QueueObject>(vk_queue, present_queue_family_index);

    return std::make_shared<DeviceObject>(vk_device, queue);
  }

  void _destroyCore(std::shared_ptr<DeviceObject> object) {
    _destroyVkDevice(object->_vk_device);
  }
};

#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "instanceObject.h"
#include "PhysicalDeviceObject.h"

class InstanceFactory {
  static const VkInstance _createVkInstance(const char* app_name, const uint32_t app_version);

  static const void _destroyVkInstance(const VkInstance instance) {
    vkDestroyInstance(instance, nullptr);
  }

  static auto _getVkPhysicalDevices(VkInstance instance) {
    uint32_t physical_device_count = 0;
    auto result = vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr); // result
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    result = vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()); // result

    return std::move(physical_devices);
  }

public:
  InstanceFactory() {
  }

  ~InstanceFactory() {
  }

  auto createInstance(const char* app_name, const uint32_t app_version) {
    auto vk_instance = _createVkInstance(app_name, app_version);
    auto vk_physical_devices = _getVkPhysicalDevices(vk_instance);

    std::vector<std::shared_ptr<PhysicalDeviceObject>> devices;
    devices.reserve(vk_physical_devices.size());
    for (auto& vk_physical_device : vk_physical_devices) {
      VkPhysicalDeviceMemoryProperties memory_properties;
      vkGetPhysicalDeviceMemoryProperties(vk_physical_device, &memory_properties);

      uint32_t count = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &count, nullptr);
      std::vector<VkQueueFamilyProperties> queue_family_properties(count);
      vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &count, queue_family_properties.data());

      auto physical_device_object = std::make_shared<PhysicalDeviceObject>(
        vk_physical_device,
        memory_properties,
        std::move(queue_family_properties));
      devices.push_back(physical_device_object);
    }

    auto object = std::make_shared<InstanceObject>(vk_instance, std::move(devices));
    _container.insert(object);
    return object;
  }

  void destroyInstance(std::shared_ptr<InstanceObject>& object) {
    if (!object) { return; }
    auto before_size = _container.size();
    _container.erase(object);
    auto after_size = _container.size();

    if (before_size != after_size) {
      _destroyVkInstance(object->_vk_instance);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<InstanceObject>> _container;
};

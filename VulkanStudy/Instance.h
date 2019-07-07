#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <memory>

constexpr auto ENGINE_NAME = "Speell";
constexpr auto ENGINE_VERSION = 0;

class InstanceObject {
public:
  InstanceObject(const VkInstance instance, const std::vector<VkPhysicalDevice> devices) :
    _vk_instance(instance),
    _devices(devices) {
  }

  ~InstanceObject() {
  }

  const VkInstance _vk_instance;
  const std::vector<VkPhysicalDevice> _devices;
protected:
private:
};

class InstanceFactory {
  static const VkInstance _createVkInstance(const char* app_name, const uint32_t app_version);

  static const void _destroyVkInstance(const VkInstance instance) {
    vkDestroyInstance(instance, nullptr);
  }

  static auto _getPhysicalDevices(VkInstance instance) {
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
    auto devices = _getPhysicalDevices(vk_instance);
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

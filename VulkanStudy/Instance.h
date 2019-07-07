#pragma once

#include <vulkan/vulkan.h>
#include <set>

#include "PhysicalDevice.h"

constexpr auto ENGINE_NAME = "Speell";
constexpr auto ENGINE_VERSION = 0;

class InstanceObject {
public:
  InstanceObject(const VkInstance instance, const std::vector<PhysicalDevice> devices) :
    _vk_instance(instance),
    _devices(devices) {
  }

  ~InstanceObject() {
  }

  const VkInstance _vk_instance;
  const std::vector<PhysicalDevice> _devices;
protected:
private:
};

class InstanceFactory {
  static const auto _createVkInstance(const char* app_name, const uint32_t app_version) {
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = app_version;
    app_info.pEngineName = ENGINE_NAME;
    app_info.engineVersion = ENGINE_VERSION;
    app_info.apiVersion = VK_API_VERSION_1_0;

    // Layers verification
    std::vector<const char*> required_instance_layers;
#ifdef _DEBUG
    required_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // DEBUG

    uint32_t layer_count = 0;
    auto result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_instance_layers(layer_count);
    result = vkEnumerateInstanceLayerProperties(&layer_count, available_instance_layers.data());

    std::vector<const char*> enabled_instance_layers;
    for (auto required : required_instance_layers) {
      for (auto allowed : available_instance_layers) {
        if (strcmp(required, allowed.layerName) == 0) {
          enabled_instance_layers.push_back(required);
          break;
        }
      }
    }

    // Extensions verification
    std::vector<const char*> required_instance_extensions;
    required_instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    required_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    uint32_t extension_count = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> availeble_instance_extensions(extension_count);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, availeble_instance_extensions.data());

    std::vector<const char*> enabled_instance_extensions;
    for (auto required : required_instance_extensions) {
      for (auto allowed : availeble_instance_extensions) {
        if (strcmp(required, allowed.extensionName) == 0) {
          enabled_instance_extensions.push_back(required);
          break;
        }
      }
    }

    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledLayerCount = static_cast<uint32_t>(enabled_instance_layers.size());
    instance_info.ppEnabledLayerNames = enabled_instance_layers.data();
    instance_info.enabledExtensionCount = static_cast<uint32_t>(enabled_instance_extensions.size());
    instance_info.ppEnabledExtensionNames = enabled_instance_extensions.data();

    VkInstance instance;
    result = vkCreateInstance(&instance_info, nullptr, &instance);
    return instance;
  }

  static const void _destroyVkInstance(const VkInstance instance) {
    vkDestroyInstance(instance, nullptr);
  }

  static auto _getPhysicalDevices(VkInstance instance) {
    uint32_t physical_device_count = 0;
    auto result = vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr); // result
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    result = vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()); // result

    std::vector<PhysicalDevice> _physical_devices;
    _physical_devices.reserve(physical_device_count);
    for (uint32_t i = 0; i < physical_device_count; i++) {
      _physical_devices.push_back(PhysicalDevice(physical_devices[i]));
    }
    return std::move(_physical_devices);
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

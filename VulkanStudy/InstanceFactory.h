#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

#include "MailingFactory.h"
#include "instanceObject.h"
#include "PhysicalDeviceObject.h"

struct InstanceParams {
  const char* app_name;
  uint32_t app_version;
  std::vector<const char*> extensions;
};

using InstanceOrder = Order<InstanceObject, InstanceParams>;
using InstanceBorrowed = std::unique_ptr<Borrowed<InstanceObject>>;

class InstanceFactory : public MailingFactory<InstanceObject, InstanceParams> {
public:
protected:
private:
  static const VkInstance _createVkInstance(const char* app_name, const uint32_t app_version, const std::vector<const char*>& extensions);

  static const void _destroyVkInstance(VkInstance instance) {
    vkDestroyInstance(instance, nullptr);
  }

  static auto _getVkPhysicalDevices(VkInstance instance) {
    uint32_t physical_device_count = 0;
    auto result = vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr); // result
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    result = vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()); // result

    return std::move(physical_devices);
  }

  std::shared_ptr<InstanceObject> _createObject(const InstanceParams& params) {
    auto vk_instance = _createVkInstance(params.app_name, params.app_version, params.extensions);
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

      VkPhysicalDeviceFeatures physical_device_features;
      vkGetPhysicalDeviceFeatures(vk_physical_device, &physical_device_features);

      auto physical_device_object = std::make_shared<PhysicalDeviceObject>(
        vk_physical_device,
        physical_device_features,
        memory_properties,
        std::move(queue_family_properties));
      devices.push_back(physical_device_object);
    }

#ifdef _DEBUG
    _vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vk_instance, "vkCreateDebugReportCallbackEXT");
    _vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugReportCallbackEXT");

    VkDebugReportCallbackCreateInfoEXT debug_report_callback_info = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
    debug_report_callback_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT;
    debug_report_callback_info.pfnCallback = [](
      VkDebugReportFlagsEXT flags,
      VkDebugReportObjectTypeEXT object_type, uint64_t object,
      size_t location, int32_t message_code, const char* layer_prefix,
      const char *message, void *user_data) -> VkBool32 {
      switch (flags) {
      case VK_DEBUG_REPORT_ERROR_BIT_EXT:
        std::cout << "VK ERROR " << layer_prefix << " " << location << ":" << message_code << ": " << message << std::endl;
        break;
      case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        std::cout << "VK WARNING " << layer_prefix << " " << location << ":" << message_code << ": " << message << std::endl;
        break;
      case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
        std::cout << "VK PERF " << layer_prefix << " " << location << ":" << message_code << ": " << message << std::endl;
        break;
      case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
        std::cout << "VK INFO " << layer_prefix << " " << location << ":" << message_code << ": " << message << std::endl;
        break;
      case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
        std::cout << "VK DEBUG " << layer_prefix << " " << location << ":" << message_code << ": " << message << std::endl;
        break;
      default:
        break;
      }
      return VK_FALSE;

    };
    debug_report_callback_info.pUserData = nullptr;

    _vkCreateDebugReportCallbackEXT(vk_instance, &debug_report_callback_info, nullptr, &_vk_debug_report_callback);
#endif // DEBUG

    return std::make_shared<InstanceObject>(vk_instance, std::move(devices));
  }

  void _returnObject(InstanceObject* object) {
#ifdef _DEBUG
    _vkDestroyDebugReportCallbackEXT(object->_vk_instance, _vk_debug_report_callback, nullptr);
#endif // DEBUG
    _destroyVkInstance(object->_vk_instance);
  }

#ifdef _DEBUG
  PFN_vkCreateDebugReportCallbackEXT _vkCreateDebugReportCallbackEXT = nullptr;
  PFN_vkDestroyDebugReportCallbackEXT _vkDestroyDebugReportCallbackEXT = nullptr;
  VkDebugReportCallbackEXT _vk_debug_report_callback;
#endif // DEBUG
};

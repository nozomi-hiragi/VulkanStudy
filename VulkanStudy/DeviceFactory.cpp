
#include "DeviceFactory.h"

VkDevice DeviceFactory::_createVkDevice(const std::shared_ptr<PhysicalDeviceObject> physical_device, uint32_t present_queue_family_index, const std::vector<const char*>& extensions) {

  float queue_prioritie = 0.f;
  VkDeviceQueueCreateInfo queue_info = {};
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.queueCount = 1;
  queue_info.queueFamilyIndex = present_queue_family_index;
  queue_info.pQueuePriorities = &queue_prioritie;

  std::vector<const char*> required_device_extensions;

  required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  required_device_extensions.reserve(required_device_extensions.size() + extensions.size());
  std::copy(extensions.begin(), extensions.end(), std::back_inserter(required_device_extensions));

  // Extensions verification
  uint32_t extension_count = 0;
  auto result = vkEnumerateDeviceExtensionProperties(physical_device->_vk_physical_device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_device_extensions(extension_count);
  result = vkEnumerateDeviceExtensionProperties(physical_device->_vk_physical_device, nullptr, &extension_count, available_device_extensions.data());
  std::vector<const char*> enabled_device_extensions;
  for (auto required : required_device_extensions) {
    for (auto allowed : available_device_extensions) {
      if (strcmp(required, allowed.extensionName) == 0) {
        enabled_device_extensions.push_back(required);
        break;
      }
    }
  }

  VkDeviceCreateInfo device_info = {};
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.queueCreateInfoCount = 1;
  device_info.pQueueCreateInfos = &queue_info;
  device_info.enabledLayerCount = 0;
  device_info.ppEnabledLayerNames = nullptr;
  device_info.enabledExtensionCount = static_cast<uint32_t>(enabled_device_extensions.size());
  device_info.ppEnabledExtensionNames = enabled_device_extensions.data();
  device_info.pEnabledFeatures = &physical_device->_vk_physical_device_features;

  VkDevice device;
  vkCreateDevice(physical_device->_vk_physical_device, &device_info, nullptr, &device);

  return device;
}

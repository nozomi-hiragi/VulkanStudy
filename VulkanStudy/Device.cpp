
#include "Device.h"

Device Device::createDevice(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface) {

  auto get_surface_support = [](VkPhysicalDevice physical_device, const uint32_t index, VkSurfaceKHR surface) {
    VkBool32 result;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, index, surface, &result);
    return result == VK_TRUE;
  };

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

  float queue_prioritie = 0.f;
  VkDeviceQueueCreateInfo queue_info = {};
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.queueCount = 1;
  queue_info.queueFamilyIndex = present_queue_family_index;
  queue_info.pQueuePriorities = &queue_prioritie;

  // Extensions verification
  std::vector<const char*> required_device_extensions;
  required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  uint32_t extension_count = 0;
  auto result = vkEnumerateDeviceExtensionProperties(physical_device->_physical_device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_device_extensions(extension_count);
  result = vkEnumerateDeviceExtensionProperties(physical_device->_physical_device, nullptr, &extension_count, available_device_extensions.data());


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
  device_info.pEnabledFeatures = nullptr;

  VkDevice vk_device;
  vkCreateDevice(physical_device->_physical_device, &device_info, nullptr, &vk_device);
  auto device = Device(vk_device);

  VkQueue vk_queue;
  vkGetDeviceQueue(vk_device, present_queue_family_index, 0, &vk_queue);
  device._queue = std::make_shared<QueueObject>(vk_queue, present_queue_family_index);

  return device;
}

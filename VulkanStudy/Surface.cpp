
#include "Surface.h"

#include "PhysicalDevice.h"

const VkSurfaceKHR SurfaceFactory::_createVkSurface(VkInstance instance, const HINSTANCE hinstance, const HWND hwnd) {
  VkWin32SurfaceCreateInfoKHR surface_info = {};
  surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surface_info.hinstance = hinstance;
  surface_info.hwnd = hwnd;

  VkSurfaceKHR surface;
  auto result = vkCreateWin32SurfaceKHR(instance, &surface_info, nullptr, &surface); // result
  return surface;
}

void Surface::fixSurfaceFormat(std::shared_ptr<PhysicalDeviceObject> physical_device) {
  uint32_t count = 0;
  auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device->_physical_device, _surface, &count, nullptr);
  std::vector<VkSurfaceFormatKHR> surface_formats(count);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device->_physical_device, _surface, &count, surface_formats.data());

  _surface_format = surface_formats[0];
  if (surface_formats.size() == 1 && _surface_format.format == VK_FORMAT_UNDEFINED) {
    _surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
  }
}

void Surface::fixSurfaceCapabilities(std::shared_ptr<PhysicalDeviceObject> physical_device) {
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device->_physical_device, _surface, &_surface_capabilities);
}

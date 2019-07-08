
#include "SurfaceFactory.h"

const VkSurfaceKHR SurfaceFactory::_createVkSurface(VkInstance instance, const HINSTANCE hinstance, const HWND hwnd) {
  VkWin32SurfaceCreateInfoKHR surface_info = {};
  surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surface_info.hinstance = hinstance;
  surface_info.hwnd = hwnd;

  VkSurfaceKHR surface;
  auto result = vkCreateWin32SurfaceKHR(instance, &surface_info, nullptr, &surface); // result
  return surface;
}

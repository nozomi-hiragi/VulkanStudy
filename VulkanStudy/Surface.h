#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

class PhysicalDevice;

class SurfaceObject {
public:
  SurfaceObject(const VkSurfaceKHR surface) : _surface(surface) {
  }

  ~SurfaceObject() {
  }

  const VkSurfaceKHR _surface;
protected:
private:
};

class SurfaceFactory {
public:
  static const auto _createVkSurface(VkInstance instance, const HINSTANCE hinstance, const HWND hwnd) {
    VkWin32SurfaceCreateInfoKHR surface_info = {};
    surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.hinstance = hinstance;
    surface_info.hwnd = hwnd;

    VkSurfaceKHR surface;
    auto result = vkCreateWin32SurfaceKHR(instance, &surface_info, nullptr, &surface); // result
    return surface;
  }

  static const void _destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
  }

  SurfaceFactory() {
  }

  ~SurfaceFactory() {
  }

protected:
private:
};

class Surface {
public:


  Surface() :
    _surface(nullptr) {
  }

  Surface(VkInstance instance, const HINSTANCE hinstance, const HWND hwnd) :
    _surface(SurfaceFactory::_createVkSurface(instance, hinstance, hwnd)) {
  }

  ~Surface() {
  }

  static auto createSurface(VkInstance instance, const HINSTANCE hinstance, const HWND hwnd) {
    return Surface(instance, hinstance, hwnd);
  }

  static void destroySurface(VkInstance instance, Surface& surface) {
    auto vk_surface = surface.getVkSurface();
    if (!vk_surface) { return; }
    SurfaceFactory::_destroyVkSurface(instance, vk_surface);
    surface.setVkSurface(nullptr);
  }

  void setVkSurface(const VkSurfaceKHR surface) {
    _surface = surface;
  }

  VkSurfaceKHR getVkSurface() {
    return _surface;
  }

  void fixSurfaceProperties(PhysicalDevice* physical_device) {
    fixSurfaceFormat(physical_device);
    fixSurfaceCapabilities(physical_device);
  }

  auto getFormat() {
    return _surface_format.format;
  }

  auto getColorSpace() {
    return _surface_format.colorSpace;
  }

  auto getSurfaceCapabilities() {
    return _surface_capabilities;
  }

protected:

private:
  void fixSurfaceFormat(PhysicalDevice* physical_device);
  void fixSurfaceCapabilities(PhysicalDevice* physical_device);

  VkSurfaceKHR _surface;
  VkSurfaceFormatKHR _surface_format;
  VkSurfaceCapabilitiesKHR _surface_capabilities;
};

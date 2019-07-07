#pragma once

#include <vulkan/vulkan.h>

class PhysicalDevice;

class Surface {
public:
  Surface() :
    _surface(nullptr),
    _linked_physical_device(nullptr) {
  }

  Surface(const vk::SurfaceKHR surface, PhysicalDevice* physical_device) :
    _surface(surface),
    _linked_physical_device(physical_device) {
    fixSurfaceFormat();
    fixSurfaceCapabilities();
  }

  ~Surface() {
  }

  void setVkSurface(const vk::SurfaceKHR surface) {
    _surface = surface;
  }

  vk::SurfaceKHR getVkSurface() {
    return _surface;
  }

  void linkPhysilacDevice(PhysicalDevice* physical_device) {
    _linked_physical_device = physical_device;
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
  void fixSurfaceFormat();
  void fixSurfaceCapabilities();

  vk::SurfaceKHR _surface;
  PhysicalDevice* _linked_physical_device;
  vk::SurfaceFormatKHR _surface_format;
  vk::SurfaceCapabilitiesKHR _surface_capabilities;
};

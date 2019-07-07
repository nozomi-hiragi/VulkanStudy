#pragma once

#include <vulkan/vulkan.h>

class PhysicalDevice;

class Surface {
public:
  Surface() :
    _surface(nullptr),
    _linked_physical_device(nullptr) {
  }

  Surface(const VkSurfaceKHR surface, PhysicalDevice* physical_device) :
    _surface(surface),
    _linked_physical_device(physical_device) {
    fixSurfaceFormat();
    fixSurfaceCapabilities();
  }

  ~Surface() {
  }

  void setVkSurface(const VkSurfaceKHR surface) {
    _surface = surface;
  }

  VkSurfaceKHR getVkSurface() {
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

  VkSurfaceKHR _surface;
  PhysicalDevice* _linked_physical_device;
  VkSurfaceFormatKHR _surface_format;
  VkSurfaceCapabilitiesKHR _surface_capabilities;
};

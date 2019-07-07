
#include "Surface.h"

#include "PhysicalDevice.h"

void Surface::fixSurfaceFormat(PhysicalDevice* physical_device) {
  auto surface_formats = physical_device->getSurfaceFormats(*this);
  _surface_format = surface_formats[0];
  if (surface_formats.size() == 1 && _surface_format.format == VK_FORMAT_UNDEFINED) {
    _surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
  }
}

void Surface::fixSurfaceCapabilities(PhysicalDevice* physical_device) {
  _surface_capabilities = physical_device->getSurfaceCapabilities(*this);
}

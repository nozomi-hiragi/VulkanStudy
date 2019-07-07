
#include "Surface.h"

#include "PhysicalDevice.h"

void Surface::fixSurfaceFormat() {
  auto surface_formats = _linked_physical_device->getSurfaceFormats(*this);
  _surface_format = surface_formats[0];
  if (surface_formats.size() == 1 && _surface_format.format == vk::Format::eUndefined) {
    _surface_format.format = vk::Format::eB8G8R8A8Unorm;
  }
}

void Surface::fixSurfaceCapabilities() {
  _surface_capabilities = _linked_physical_device->getSurfaceCapabilities(*this);
}

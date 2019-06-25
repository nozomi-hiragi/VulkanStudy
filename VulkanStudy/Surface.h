#pragma once

#include <vulkan/vulkan.hpp>

class Surface {
public:
  Surface(): _surface(nullptr) {
  }

  Surface(const vk::SurfaceKHR surface): _surface(surface) {
  }

  ~Surface() {
  }

  void setVkSurface(const vk::SurfaceKHR surface) {
    _surface = surface;
  }

  vk::SurfaceKHR getVkSurface() {
    return _surface;
  }

protected:

private:
  vk::SurfaceKHR _surface;
};

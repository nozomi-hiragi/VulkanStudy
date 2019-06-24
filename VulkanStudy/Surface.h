#pragma once

#include <vulkan/vulkan.hpp>

class Surface {
public:
  Surface(): _surface(nullptr) {
  }

  Surface(vk::SurfaceKHR surface): _surface(surface) {
  }

  ~Surface() {
  }

  void setVkSurface(vk::SurfaceKHR surface) {
    _surface = surface;
  }

  vk::SurfaceKHR getVkSurface() {
    return _surface;
  }

protected:

private:
  vk::SurfaceKHR _surface;
};

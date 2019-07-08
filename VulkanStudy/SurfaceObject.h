#pragma once

#include <vulkan/vulkan.h>

class SurfaceObject {
public:
  SurfaceObject(const VkSurfaceKHR surface): _vk_surface(surface) {
  }

  ~SurfaceObject() {
  }

  const VkSurfaceKHR _vk_surface;
protected:
private:
};

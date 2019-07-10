#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "SurfaceObject.h"
#include "InstanceObject.h"

class SurfaceFactory : public AbstractFactory<SurfaceObject, InstanceObject, const HINSTANCE, const HWND>{
public:
  SurfaceFactory() {
  }

  ~SurfaceFactory() {
  }

protected:
private:
  static const VkSurfaceKHR _createVkSurface(VkInstance instance, const HINSTANCE hinstance, const HWND hwnd);

  static const void _destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
  }

  std::shared_ptr<SurfaceObject> _createCore(const HINSTANCE hinstance, const HWND hwnd) {
    auto vk_surface = _createVkSurface(_parent->_vk_instance, hinstance, hwnd);
    return std::make_shared<SurfaceObject>(vk_surface);
  }

  void _destroyCore(std::shared_ptr<SurfaceObject> object) {
    _destroyVkSurface(_parent->_vk_instance, object->_vk_surface);
  }
};

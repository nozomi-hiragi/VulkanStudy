#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <memory>

#include "Instance.h"

class PhysicalDeviceObject;

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

class SurfaceFactory {
  static const VkSurfaceKHR _createVkSurface(VkInstance instance, const HINSTANCE hinstance, const HWND hwnd);

  static const void _destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
  }

public:
  SurfaceFactory() {
  }

  ~SurfaceFactory() {
  }

  auto createSurface(std::shared_ptr<InstanceObject> instance, const HINSTANCE hinstance, const HWND hwnd) {
    auto vk_surface = _createVkSurface(instance->_vk_instance, hinstance, hwnd);
    auto object = std::make_shared<SurfaceObject>(vk_surface);
    return object;
  }

  void destroySurface(std::shared_ptr<InstanceObject> instance, std::shared_ptr<SurfaceObject>& object) {
    if (!object) { return; }
    auto before_size = _container.size();
    _container.erase(object);
    auto after_size = _container.size();

    if (before_size != after_size) {
      _destroyVkSurface(instance->_vk_instance, object->_vk_surface);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<SurfaceObject>> _container;
};

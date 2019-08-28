#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <memory>
#include <set>
#include <GLFW/glfw3.h>

#include "StandardFactory.h"
#include "SurfaceObject.h"
#include "InstanceObject.h"

class SurfaceFactory : public StandardFactory<SurfaceObject, InstanceObject, GLFWwindow*> {
public:
  SurfaceFactory() {
  }

  ~SurfaceFactory() {
  }

protected:
private:
  static const VkSurfaceKHR _createVkSurface(VkInstance instance, GLFWwindow* window) {
    VkSurfaceKHR vk_surface;
    VkResult err = glfwCreateWindowSurface(instance, window, nullptr, &vk_surface);
    return vk_surface;
  }

  static const void _destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
  }

  std::shared_ptr<SurfaceObject> _createCore(GLFWwindow* window) {
    auto vk_surface = _createVkSurface(_parent->_vk_instance, window);
    return std::make_shared<SurfaceObject>(vk_surface);
  }

  void _destroyCore(std::shared_ptr<SurfaceObject> object) {
    _destroyVkSurface(_parent->_vk_instance, object->_vk_surface);
  }
};

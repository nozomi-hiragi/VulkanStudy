#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <memory>
#include <set>
#include <GLFW/glfw3.h>

#include "MailingFactory.h"
#include "SurfaceObject.h"
#include "InstanceObject.h"

struct SurfaceParams {
  std::shared_ptr<InstanceObject> instiace;
  GLFWwindow* window;
};

class SurfaceFactory : public MailingFactory<SurfaceObject, SurfaceParams, std::shared_ptr<InstanceObject>> {
public:
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

  std::shared_ptr<SurfaceObject> _createObject(SurfaceParams& params) {
    auto vk_surface = _createVkSurface(params.instiace->_vk_instance, params.window);
    return std::make_shared<SurfaceObject>(vk_surface);
  }

  void _returnObject(SurfaceObject* object, std::shared_ptr<InstanceObject> parent) {
    _destroyVkSurface(parent->_vk_instance, object->_vk_surface);
  }
};

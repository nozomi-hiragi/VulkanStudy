#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <memory>
#include <queue>
#include <GLFW/glfw3.h>

#include "MailingFactory.h"
#include "SurfaceObject.h"
#include "InstanceObject.h"

struct SurfaceParams {
  std::shared_ptr<InstanceObject> instiace;
  GLFWwindow* window;
};

using SurfaceOrder = Order<SurfaceObject, SurfaceParams>;
using SurfaceBorrowed = std::unique_ptr<Borrowed<SurfaceObject>>;

class SurfaceFactory : public MailingFactory<SurfaceObject, SurfaceParams> {
public:
  void executeDestroy(std::shared_ptr<InstanceObject> instance) {
    while (!_destroy_queue.empty()) {
      _destroyVkSurface(instance->_vk_instance, _destroy_queue.front());
      _destroy_queue.pop();
    }
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

  std::shared_ptr<SurfaceObject> _createObject(const SurfaceParams& params) {
    auto vk_surface = _createVkSurface(params.instiace->_vk_instance, params.window);
    return std::make_shared<SurfaceObject>(vk_surface);
  }

  void _returnObject(SurfaceObject* object) {
    _destroy_queue.push(object->_vk_surface);
  }

  std::queue<VkSurfaceKHR> _destroy_queue;
};

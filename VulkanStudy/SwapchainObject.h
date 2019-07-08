#pragma once

#include <vulkan/vulkan.h>

class SwapchainObject {
public:
  SwapchainObject(
    const VkSwapchainKHR swapchain,
    const VkFormat format,
    const VkColorSpaceKHR color_space) :
    _swapchain(swapchain),
    _format(format),
    _color_space(color_space) {
  }

  ~SwapchainObject() {
  }

  const VkSwapchainKHR _swapchain;
  const VkFormat _format;
  const VkColorSpaceKHR _color_space;
protected:
private:
};

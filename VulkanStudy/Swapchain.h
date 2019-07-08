#pragma once

#include <vulkan/vulkan.hpp>

class Swapchain {
public:
  Swapchain() : _swapchain(nullptr) {
  }

  explicit Swapchain(const vk::SwapchainKHR swapchain, VkSurfaceFormatKHR surface_format):
    _swapchain(swapchain),
    _surface_format(surface_format) {
  }

  ~Swapchain() {
  }

  void setVkSwapchain(const vk::SwapchainKHR swapchain) {
    _swapchain = swapchain;
  }

  const auto getVkSwapchain() {
    return _swapchain;
  }

  auto getFormat() {
    return _surface_format.format;
  }

protected:
private:
  vk::SwapchainKHR _swapchain;
  VkSurfaceFormatKHR _surface_format;
};

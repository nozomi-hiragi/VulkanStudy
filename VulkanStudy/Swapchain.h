#pragma once

#include <vulkan/vulkan.hpp>

class Swapchain {
public:
  Swapchain() : _swapchain(nullptr) {
  }

  explicit Swapchain(const vk::SwapchainKHR swapchain) : _swapchain(swapchain) {
  }

  ~Swapchain() {
  }

  void setVkSwapchain(const vk::SwapchainKHR swapchain) {
    _swapchain = swapchain;
  }

  const auto getVkSwapchain() {
    return _swapchain;
  }

protected:
private:
  vk::SwapchainKHR _swapchain;
};

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "ImageObject.h"
#include "DeviceObject.h"
#include "SemaphoreObject.h"
#include "FenceObject.h"

class SwapchainObject {
public:
  SwapchainObject(
    const VkSwapchainKHR swapchain,
    const VkFormat format,
    const VkColorSpaceKHR color_space,
    const std::vector<std::shared_ptr<ImageObject>> swapchain_images) :
    _vk_swapchain(swapchain),
    _vk_format(format),
    _vk_color_space(color_space),
    _swapchain_images(swapchain_images),
    _swapchain_image_count(static_cast<uint32_t>(swapchain_images.size())) {
  }

  ~SwapchainObject() {
  }

  void acquireNextImage(const std::shared_ptr<DeviceObject> device, const uint64_t timeout, const std::shared_ptr<SemaphoreObject> semaphore, const std::shared_ptr<FenceObject> fence, uint32_t* image_index) {
    VkFence vk_fence = fence ? fence->_vk_fence : nullptr;
    vkAcquireNextImageKHR(device->_vk_device, _vk_swapchain, timeout, semaphore->_vk_semaphore, vk_fence, image_index);
  }

  const VkSwapchainKHR _vk_swapchain;
  const VkFormat _vk_format;
  const VkColorSpaceKHR _vk_color_space;
  const std::vector<std::shared_ptr<ImageObject>> _swapchain_images;
  const uint32_t _swapchain_image_count;
protected:
private:
};

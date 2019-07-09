#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "SwapchainObject.h"

class SwapchainFactory {
  static VkSurfaceCapabilitiesKHR _getSurfaceCapabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);
    return surface_capabilities;
  }

  static VkSurfaceFormatKHR _getFixSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    uint32_t count = 0;
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(count);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, surface_formats.data());

    auto surface_format = surface_formats[0];
    if (surface_formats.size() == 1 && surface_format.format == VK_FORMAT_UNDEFINED) {
      surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    return surface_format;
  }

  static std::vector<VkImage> _getVkSwapchainImages(VkDevice device, VkSwapchainKHR swapchain) {
    uint32_t swapchain_image_count = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
    std::vector<VkImage> swapchain_images(swapchain_image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data());
    return std::move(swapchain_images);
  }

  static VkSwapchainKHR _createVkSwapchain(VkDevice device, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR surface_capabilities, VkSurfaceFormatKHR surface_format, const uint32_t width, const uint32_t height) {

    VkSurfaceTransformFlagBitsKHR pre_transform;
    if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
      pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else {
      pre_transform = surface_capabilities.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR composite_alpha_flags[] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (auto it : composite_alpha_flags) {
      if (surface_capabilities.supportedCompositeAlpha & it) {
        composite_alpha = it;
        break;
      }
    }

    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = surface;
    swapchain_info.minImageCount = surface_capabilities.minImageCount;
    swapchain_info.imageFormat = surface_format.format;
    swapchain_info.imageColorSpace = surface_format.colorSpace;
    swapchain_info.imageExtent = { width, height };
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_info.queueFamilyIndexCount = 0; // Ç†ÇÍÅH
    swapchain_info.pQueueFamilyIndices = nullptr;
    swapchain_info.preTransform = pre_transform;
    swapchain_info.compositeAlpha = composite_alpha;
    swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_info.clipped = true;
    swapchain_info.oldSwapchain = nullptr;

    VkSwapchainKHR swapchain;
    auto result = vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain);

    return swapchain;
  }

  static void _destroyVkSwapchain(VkDevice device, VkSwapchainKHR swapchain) {
    vkDestroySwapchainKHR(device, swapchain, nullptr);
  }

  static auto _getVkImageMemoryRequirements(VkDevice device, VkImage image) {
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, image, &memory_requirements);
    return std::move(memory_requirements);
  }

public:
  std::shared_ptr<SwapchainObject> createSwapchain(VkDevice device, VkSurfaceKHR surface, VkPhysicalDevice physical_device, const uint32_t width, const uint32_t height) {
    auto surface_capabilities = _getSurfaceCapabilities(physical_device, surface);
    auto surface_format = _getFixSurfaceFormat(physical_device, surface);
    auto vk_swapchain = _createVkSwapchain(device, surface, surface_capabilities, surface_format, width, height);
    auto vk_swapchain_images = _getVkSwapchainImages(device, vk_swapchain);

    std::vector<std::shared_ptr<ImageObject>> swapchain_images;
    swapchain_images.reserve(vk_swapchain_images.size());
    for (auto it : vk_swapchain_images) {
      auto memory_requirements = _getVkImageMemoryRequirements(device, it);
      swapchain_images.push_back(std::make_shared<ImageObject>(it, std::move(memory_requirements), surface_format.format));
    }

    auto object = std::make_shared<SwapchainObject>(
      vk_swapchain,
      surface_format.format,
      surface_format.colorSpace,
      std::move(swapchain_images));
    _container.insert(object);
    return object;
  }

  void destroySwapchain(VkDevice device, std::shared_ptr<SwapchainObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyVkSwapchain(device, object->_vk_swapchain);
      object.reset();
    }
  }

  SwapchainFactory() {
  }

  ~SwapchainFactory() {
  }

protected:
private:
  std::set<std::shared_ptr<SwapchainObject>> _container;
};

#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "SwapchainObject.h"
#include "DeviceObject.h"
#include "PhysicalDeviceObject.h"
#include "SurfaceObject.h"

class SwapchainFactory : public AbstractFactory<SwapchainObject, DeviceObject, const std::shared_ptr<PhysicalDeviceObject>, const std::shared_ptr<SurfaceObject>, const uint32_t, const uint32_t, const bool> {
public:
  SwapchainFactory() {
  }

  ~SwapchainFactory() {
  }

  auto createObject(const std::shared_ptr<DeviceObject> parent, const std::shared_ptr<PhysicalDeviceObject> physical_device, const std::shared_ptr<SurfaceObject> surface, const uint32_t width, const uint32_t height, const bool is_srgb = false) {
    return AbstractFactory::createObject(parent, physical_device, surface, width, height, is_srgb);
  }

protected:
private:
  static VkSurfaceFormatKHR _getFixSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, bool is_srgb) {
    uint32_t count = 0;
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(count);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, surface_formats.data());

    auto surface_format = surface_formats[0];
    if (count == 1 && surface_format.format == VK_FORMAT_UNDEFINED) {
      surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
    }

    if (is_srgb) {
      for (auto& it : surface_formats) {
        if (it.format != VK_FORMAT_B8G8R8A8_SRGB && it.format != VK_FORMAT_R8G8B8A8_SRGB) {
          continue;
        }
        surface_format = it;
        break;
      }
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

  static VkSwapchainKHR _createVkSwapchain(VkDevice device, VkPhysicalDevice physical_device, VkSurfaceKHR surface, VkSurfaceFormatKHR surface_format, const uint32_t width, const uint32_t height) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

    constexpr uint32_t REQ_MIN_IMAGE_COUNT = 2;
    uint32_t min_image_count = surface_capabilities.minImageCount > REQ_MIN_IMAGE_COUNT ? surface_capabilities.minImageCount : REQ_MIN_IMAGE_COUNT;
    if ((surface_capabilities.maxImageCount > 0) && (min_image_count > surface_capabilities.maxImageCount)) {
      min_image_count = surface_capabilities.maxImageCount;
    }

    VkExtent2D swapchain_extent;
    if (surface_capabilities.currentExtent.width == -1) {
      swapchain_extent.width = width;
      swapchain_extent.height = height;
    } else {
      swapchain_extent = surface_capabilities.currentExtent;
    }

    VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
      image_usage_flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    VkSurfaceTransformFlagBitsKHR pre_transform;
    if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
      pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
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

    uint32_t present_mode_count;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
    std::vector<VkPresentModeKHR > present_modes(present_mode_count);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto& it : present_modes) {
      if (it == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        present_mode = it;
        break;
      } else if (it == VK_PRESENT_MODE_MAILBOX_KHR) {
        present_mode = it;
      } else if ((it == VK_PRESENT_MODE_MAILBOX_KHR) && present_mode != VK_PRESENT_MODE_MAILBOX_KHR) {
        present_mode = it;
      }
    }

    VkSwapchainCreateInfoKHR swapchain_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchain_info.surface = surface;
    swapchain_info.minImageCount = min_image_count;
    swapchain_info.imageFormat = surface_format.format;
    swapchain_info.imageColorSpace = surface_format.colorSpace;
    swapchain_info.imageExtent = swapchain_extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = image_usage_flags;
    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_info.queueFamilyIndexCount = 0;
    swapchain_info.pQueueFamilyIndices = nullptr;
    swapchain_info.preTransform = pre_transform;
    swapchain_info.compositeAlpha = composite_alpha;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = nullptr;

    VkSwapchainKHR swapchain;
    result = vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain);

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

  std::shared_ptr<SwapchainObject> _createCore(const std::shared_ptr<PhysicalDeviceObject> physical_device, const std::shared_ptr<SurfaceObject> surface, const uint32_t width, const uint32_t height, const bool is_srgb) {
    auto surface_format = _getFixSurfaceFormat(physical_device->_vk_physical_device, surface->_vk_surface, is_srgb);
    auto vk_swapchain = _createVkSwapchain(_parent->_vk_device, physical_device->_vk_physical_device, surface->_vk_surface, surface_format, width, height);
    auto vk_swapchain_images = _getVkSwapchainImages(_parent->_vk_device, vk_swapchain);

    std::vector<std::shared_ptr<ImageObject>> swapchain_images;
    swapchain_images.reserve(vk_swapchain_images.size());
    for (auto it : vk_swapchain_images) {
      auto memory_requirements = _getVkImageMemoryRequirements(_parent->_vk_device, it);
      swapchain_images.push_back(std::make_shared<ImageObject>(it, std::move(memory_requirements), surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, width, height, 1, VK_IMAGE_LAYOUT_UNDEFINED));
    }

    return std::make_shared<SwapchainObject>(
      vk_swapchain,
      surface_format.format,
      surface_format.colorSpace,
      std::move(swapchain_images));
  }

  void _destroyCore(std::shared_ptr<SwapchainObject> object) {
    _destroyVkSwapchain(_parent->_vk_device, object->_vk_swapchain);
  }

};

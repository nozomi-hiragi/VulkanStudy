#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "AbstractFactory.h"
#include "ImageObject.h"
#include "DeviceObject.h"

class ImageFactory : public AbstractFactory<ImageObject, DeviceObject, const VkFormat, const VkImageUsageFlags, const uint32_t, const uint32_t, const VkImageAspectFlags> {
public:
  ImageFactory() {
  }

  ~ImageFactory() {
  }

protected:
private:
  static auto _createVkImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height) {
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = nullptr;
    image_info.flags = 0;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = format;
    image_info.extent = { width, height, 1};
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling = format == VK_FORMAT_D16_UNORM ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = nullptr;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image;
    auto result = vkCreateImage(device, &image_info, nullptr, &image);
    return image;
  }

  static void _destroyVkImage(VkDevice device, VkImage image) {
    vkDestroyImage(device, image, nullptr);
  }

  static auto _getVkImageMemoryRequirements(VkDevice device, VkImage image) {
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, image, &memory_requirements);
    return std::move(memory_requirements);
  }

  std::shared_ptr<ImageObject> _createCore(VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, VkImageAspectFlags image_aspect_flags) {
    auto vk_image = _createVkImage(_parent->_vk_device, format, usage, width, height);
    auto memory_requirements = _getVkImageMemoryRequirements(_parent->_vk_device, vk_image);
    return std::make_shared<ImageObject>(vk_image, std::move(memory_requirements), format, image_aspect_flags, width, height);
  }

  void _destroyCore(std::shared_ptr<ImageObject> object) {
    _destroyVkImage(_parent->_vk_device, object->_vk_image);
  }

};

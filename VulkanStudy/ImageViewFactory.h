#pragma once

#include <vulkan/vulkan.h>

#include "AbstractFactory.h"
#include "ImageViewObject.h"
#include "ImageObject.h"
#include "DeviceObject.h"

class ImageViewFactory : public AbstractFactory<ImageViewObject, DeviceObject, const std::shared_ptr<ImageObject>> {
public:
  ImageViewFactory() {
  }

  ~ImageViewFactory() {
  }

protected:
private:
  static VkImageView _createVkImageView(VkDevice device, std::shared_ptr<ImageObject> image, const VkImageAspectFlags image_aspect_flags) {
    VkImageViewCreateInfo image_view_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    image_view_info.flags = 0;
    image_view_info.image = image->_vk_image;
    image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format = image->_vk_format;
    image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    image_view_info.subresourceRange.aspectMask = image_aspect_flags;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = image->_mip_level;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    auto result = vkCreateImageView(device, &image_view_info, nullptr, &image_view);
    return image_view;
  }

  static void _destroyVkImageView(VkDevice device, VkImageView image_view) {
    vkDestroyImageView(device, image_view, nullptr);
  }

  std::shared_ptr<ImageViewObject> _createCore(std::shared_ptr<ImageObject> image) {
    auto vk_image_view = _createVkImageView(_parent->_vk_device, image, image->_vk_image_aspect_flags);
    return std::make_shared<ImageViewObject>(vk_image_view, image);
  }

  void _destroyCore(std::shared_ptr<ImageViewObject> object) {
    _destroyVkImageView(_parent->_vk_device, object->_vk_image_view);
  }

};

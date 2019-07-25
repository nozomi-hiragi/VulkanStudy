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
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = image_aspect_flags;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = 1;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount = 1;

    VkImageViewCreateInfo image_view_info = {};
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format = image->_vk_format;
    image_view_info.subresourceRange = subresource_range;
    image_view_info.image = image->_vk_image;

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

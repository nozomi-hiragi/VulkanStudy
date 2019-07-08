#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "ImageViewObject.h"
#include "ImageObject.h"

class ImageViewFactory {
  static VkImageView _createVkImageView(VkDevice device, std::shared_ptr<ImageObject> image, VkImageSubresourceRange& subresource_range) {
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

public:
  ImageViewFactory() {
  }

  ~ImageViewFactory() {
  }

  auto createImageView(VkDevice device, std::shared_ptr<ImageObject> image, VkImageSubresourceRange& subresource_range) {
    auto vk_image_view = _createVkImageView(device, image, subresource_range);
    auto object = std::make_shared<ImageViewObject>(vk_image_view);
    _container.insert(object);
    return object;
  }

  void destroyImageView(VkDevice device, std::shared_ptr<ImageViewObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyVkImageView(device, object->_vk_image_view);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<ImageViewObject>> _container;
};

#pragma once

#include <vulkan/vulkan.h>

class ImageViewObject {
public:
  ImageViewObject(const VkImageView image_view) : _vk_image_view(image_view) {
  }

  ~ImageViewObject() {
  }

  const VkImageView _vk_image_view;
protected:
private:
};

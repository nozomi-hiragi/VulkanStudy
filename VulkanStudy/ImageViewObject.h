#pragma once

#include <vulkan/vulkan.h>

class ImageObject;

class ImageViewObject {
public:
  ImageViewObject(const VkImageView image_view, const std::shared_ptr<ImageObject> image_object) :
    _vk_image_view(image_view), _image_object(image_object) {
  }

  ~ImageViewObject() {
  }

  const VkImageView _vk_image_view;
  const std::shared_ptr<ImageObject> _image_object;
protected:
private:
};

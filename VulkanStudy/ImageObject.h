#pragma once

#include <vulkan/vulkan.h>

class ImageObject {
public:
  ImageObject(const VkImage image, const VkFormat format) :
    _vk_image(image),
    _vk_format(format) {
  }

  ~ImageObject() {
  }

  const VkImage _vk_image;
  const VkFormat _vk_format;
protected:
private:
};

#pragma once

#include <vulkan/vulkan.h>

class ImageObject {
public:
  ImageObject(const VkImage image, const VkMemoryRequirements memory_requirements, const VkFormat format) :
    _vk_image(image),
    _vk_format(format),
    _memory_requirements(memory_requirements) {
  }

  ~ImageObject() {
  }

  const VkImage _vk_image;
  const VkMemoryRequirements _memory_requirements;
  const VkFormat _vk_format;
protected:
private:
};

#pragma once

#include <vulkan/vulkan.h>

#include "DeviceObject.h"
#include "DeviceMemoryObject.h"

class ImageObject {
public:
  ImageObject(const VkImage image, const VkMemoryRequirements memory_requirements, const VkFormat format, const VkImageAspectFlags image_aspect_flags) :
    _vk_image(image),
    _vk_format(format),
    _vk_memory_requirements(memory_requirements),
    _vk_image_aspect_flags(image_aspect_flags) {
  }

  ~ImageObject() {
  }

  void bindImageMemory(std::shared_ptr<DeviceObject> device, const std::shared_ptr<DeviceMemoryObject> memory, const uint64_t offset) {
    vkBindImageMemory(device->_vk_device, _vk_image, memory->_vk_device_memory, offset);
  }

  const VkImage _vk_image;
  const VkMemoryRequirements _vk_memory_requirements;
  const VkFormat _vk_format;
  const VkImageAspectFlags _vk_image_aspect_flags;
protected:
private:
};

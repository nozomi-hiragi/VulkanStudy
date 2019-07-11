#pragma once

#include <vulkan/vulkan.h>

class FramebufferObject {
public:
  FramebufferObject(VkFramebuffer framebuffer) : _vk_framebuffer(framebuffer) {
  }

  const VkFramebuffer _vk_framebuffer;
protected:
private:
};

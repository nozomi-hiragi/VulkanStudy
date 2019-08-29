#pragma once

#include <vulkan/vulkan.h>
#include <map>
#include <string>

class RenderPassObject {
public:
  RenderPassObject(const VkRenderPass vk_render_pass)
    : _vk_render_pass(vk_render_pass) {
  }

  const VkRenderPass _vk_render_pass;
protected:
private:
};

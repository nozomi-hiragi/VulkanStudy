#pragma once

#include <vulkan/vulkan.h>
#include <map>
#include <string>

class RenderPassObject {
public:
  RenderPassObject(const VkRenderPass vk_render_pass, const std::map<std::string, uint32_t> attachment_index) :
    _vk_render_pass(vk_render_pass),
    _attachment_index(attachment_index) {
  }

  const VkRenderPass _vk_render_pass;
  const std::map<std::string, uint32_t> _attachment_index;
protected:
private:
};

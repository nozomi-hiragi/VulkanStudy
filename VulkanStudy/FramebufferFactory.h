#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "FramebufferObject.h"

class FramebufferFactory {
  static VkFramebuffer _createVkFramebuffer(VkDevice device, VkRenderPass render_pass, VkImageView color_image_view, VkImageView depth_image_view, uint32_t width, uint32_t height) {
    VkImageView attachments[2];
    attachments[0] = color_image_view;
    attachments[1] = depth_image_view;

    VkFramebufferCreateInfo framebuffer_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = 2;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = width;
    framebuffer_info.height = height;
    framebuffer_info.layers = 1;

    VkFramebuffer framebuffer;
    auto result = vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer);
    return framebuffer;
  }

  static void _destroyVkFramebuffer(VkDevice device, VkFramebuffer framebuffer) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  std::shared_ptr<FramebufferObject> _createCore(VkDevice device, VkRenderPass render_pass, VkImageView color_image_view, VkImageView depth_image_view, uint32_t width, uint32_t height) {
    auto vk_framebuffer = _createVkFramebuffer(device, render_pass, color_image_view, depth_image_view, width, height);
    return std::make_shared<FramebufferObject>(vk_framebuffer);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<FramebufferObject> object) {
    _destroyVkFramebuffer(device, object->_vk_framebuffer);
  }

public:
  std::shared_ptr<FramebufferObject> createObject(VkDevice device, VkRenderPass render_pass, VkImageView color_image_view, VkImageView depth_image_view, uint32_t width, uint32_t height) {
    auto object = _createCore(device, render_pass, color_image_view, depth_image_view, width, height);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<FramebufferObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyCore(device, object);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<FramebufferObject>> _container;
};

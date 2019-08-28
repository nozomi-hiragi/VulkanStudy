#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "StandardFactory.h"
#include "FramebufferObject.h"
#include "DeviceObject.h"
#include "RenderPassObject.h"
#include "ImageViewObject.h"
#include "ImageObject.h"

class FramebufferFactory : public StandardFactory<FramebufferObject, DeviceObject, const std::shared_ptr<RenderPassObject>, const std::vector<std::shared_ptr<ImageViewObject>>&> {
  static VkFramebuffer _createVkFramebuffer(VkDevice device, VkRenderPass render_pass, const std::vector<std::shared_ptr<ImageViewObject>>& image_views) {
    if (image_views.size() <= 0) {
      return nullptr;
    }

    std::vector<VkImageView> attachments;
    attachments.reserve(image_views.size());
    for (const auto& it : image_views) {
      attachments.push_back(it->_vk_image_view);
    }

    VkFramebufferCreateInfo framebuffer_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = image_views[0]->_image_object->_width;
    framebuffer_info.height = image_views[0]->_image_object->_height;
    framebuffer_info.layers = 1;

    VkFramebuffer framebuffer;
    auto result = vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer);
    return framebuffer;
  }

  static void _destroyVkFramebuffer(VkDevice device, VkFramebuffer framebuffer) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  std::shared_ptr<FramebufferObject> _createCore(const std::shared_ptr<RenderPassObject> render_pass, const std::vector<std::shared_ptr<ImageViewObject>>& image_views) {
    auto vk_framebuffer = _createVkFramebuffer(_parent->_vk_device, render_pass->_vk_render_pass, image_views);
    return std::make_shared<FramebufferObject>(vk_framebuffer);
  }

  void _destroyCore(std::shared_ptr<FramebufferObject> object) {
    _destroyVkFramebuffer(_parent->_vk_device, object->_vk_framebuffer);
  }

public:
protected:
private:
};

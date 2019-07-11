#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "RenderPassObject.h"

class RenderPassFactory {
  static VkRenderPass _createVkRenderPass(VkDevice device, VkFormat color_format, VkFormat depth_format) {
    VkAttachmentDescription attachment_description[] = {
      {
        0,
        color_format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      },

      {
        0,
        depth_format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      }
    };

    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = { VK_PIPELINE_BIND_POINT_GRAPHICS };
    subpass_description.inputAttachmentCount = 0;
    subpass_description.pInputAttachments = nullptr;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_reference;
    subpass_description.pResolveAttachments = nullptr;
    subpass_description.pDepthStencilAttachment = &depth_reference;
    subpass_description.preserveAttachmentCount = 0;
    subpass_description.pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo render_pass_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = attachment_description;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass_description;
    render_pass_info.dependencyCount = 0;
    render_pass_info.pDependencies = nullptr;

    VkRenderPass render_pass;
    auto result = vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass);
    return render_pass;
  }

  static void _destroyVkRenderPass(VkDevice device, const VkRenderPass render_pass) {
    vkDestroyRenderPass(device, render_pass, nullptr);
  }

  std::shared_ptr<RenderPassObject> _createCore(VkDevice device, VkFormat color_format, VkFormat depth_format) {
    auto vk_render_pass = _createVkRenderPass(device, color_format, depth_format);
    return std::make_shared<RenderPassObject>(vk_render_pass);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<RenderPassObject> object) {
    _destroyVkRenderPass(device, object->_vk_render_pass);
  }

public:
  std::shared_ptr<RenderPassObject> createObject(VkDevice device, VkFormat color_format, VkFormat depth_format) {
    auto object = _createCore(device, color_format, depth_format);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<RenderPassObject>& object) {
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
  std::set<std::shared_ptr<RenderPassObject>> _container;
};

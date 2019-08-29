#pragma once

#include <vulkan/vulkan.h>

#include "MailingFactory.h"
#include "RenderPassObject.h"
#include "DeviceObject.h"

struct RenderPassParams {
  std::shared_ptr<DeviceObject> device;
  uint32_t description_count;
  VkAttachmentDescription* descriptions;
  uint32_t subpass_count;
  VkSubpassDescription* subpasses;
};

using RenderPassOrder = Order<RenderPassObject, RenderPassParams>;
using RenderPassBorrowed = std::unique_ptr<Borrowed<RenderPassObject>>;

class RenderPassFactory : public MailingFactory<RenderPassObject, RenderPassParams> {
public:
  void executeDestroy(std::shared_ptr<DeviceObject> device) {
    while (!_destroy_queue.empty()) {
      _destroyVkRenderPass(device->_vk_device, _destroy_queue.front());
      _destroy_queue.pop();
    }
  }

protected:
private:
  static VkRenderPass _createVkRenderPass(VkDevice device, const VkRenderPassCreateInfo& render_pass_info) {
    VkRenderPass render_pass;
    auto result = vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass);
    return render_pass;
  }

  static void _destroyVkRenderPass(VkDevice device, const VkRenderPass render_pass) {
    vkDestroyRenderPass(device, render_pass, nullptr);
  }

  std::shared_ptr<RenderPassObject> _createObject(const RenderPassParams& params) {

    VkRenderPassCreateInfo render_pass_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    render_pass_info.attachmentCount = params.description_count;
    render_pass_info.pAttachments = params.descriptions;
    render_pass_info.subpassCount = params.subpass_count;
    render_pass_info.pSubpasses = params.subpasses;
    render_pass_info.dependencyCount = 0;
    render_pass_info.pDependencies = nullptr;

    auto vk_render_pass = _createVkRenderPass(params.device->_vk_device, render_pass_info);
    return std::make_shared<RenderPassObject>(vk_render_pass);
  }

  void _returnObject(RenderPassObject* object) {
    _destroy_queue.push(object->_vk_render_pass);
  }

  std::queue<VkRenderPass> _destroy_queue;
};

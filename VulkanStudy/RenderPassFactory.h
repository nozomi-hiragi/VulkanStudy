#pragma once

#include <vulkan/vulkan.h>

#include "AbstractFactory.h"
#include "RenderPassObject.h"
#include "DeviceObject.h"

#include "RenderPassInfoDeport.h"

class RenderPassFactory : public AbstractFactory<RenderPassObject, DeviceObject, const std::vector<std::string>&> {
public:
  auto& getAttachmentDescriptionDepot() {
    return _attachment_description_depot;
  }
  auto& getAttachmentReferenceDepot() {
    return _attachment_reference_depot;
  }
  auto& getSubpassDescriptionDepot() {
    return _subpass_description_depot;
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

  std::shared_ptr<RenderPassObject> _createCore(const std::vector<std::string>& subpass_description_names) {

    std::map<std::string, uint32_t> attachment_description_index;
    std::vector<std::shared_ptr<std::vector<VkAttachmentReference>>> tmp_input_attachment_references;
    std::vector<std::shared_ptr<std::vector<VkAttachmentReference>>> tmp_color_attachment_references;
    std::vector<std::shared_ptr<std::vector<VkAttachmentReference>>> tmp_resolve_attachment_references;
    std::vector<std::shared_ptr<VkAttachmentReference>> tmp_depth_attachment_references;
    tmp_input_attachment_references.reserve(subpass_description_names.size());
    tmp_color_attachment_references.reserve(subpass_description_names.size());
    tmp_resolve_attachment_references.reserve(subpass_description_names.size());
    tmp_depth_attachment_references.reserve(subpass_description_names.size());

    auto create_vk_subpass_description = [this,
      &attachment_description_index,
      &tmp_input_attachment_references,
      &tmp_color_attachment_references,
      &tmp_resolve_attachment_references,
      &tmp_depth_attachment_references
    ](const std::string name) {
      auto arg_subpass = _subpass_description_depot.get(name);
      auto convert_to_vk_attachment_reference = [this, &attachment_description_index](const uint32_t count, const char*const* names) {
        if (count <= 0 || names == nullptr) {
          return std::move(std::make_shared<std::vector<VkAttachmentReference>>());
        }

        auto attachment_references = std::make_shared<std::vector<VkAttachmentReference>>();
        attachment_references->reserve(count);
        for (uint32_t i = 0; i < count; i++) {
          const auto& arg_attachment_reference = _attachment_reference_depot.get(names[i]);
          if (attachment_description_index.find(arg_attachment_reference.attachment) == attachment_description_index.end()) {
            auto index_pair = std::pair<std::string, uint32_t>(arg_attachment_reference.attachment, static_cast<uint32_t>(attachment_description_index.size()));
            attachment_description_index.insert(index_pair);
          }
          attachment_references->push_back(
            {
              attachment_description_index[arg_attachment_reference.attachment],
              arg_attachment_reference.layout
            }
          );
        }
        return std::move(attachment_references);
      };

      auto input_attachment_references = convert_to_vk_attachment_reference(arg_subpass.input_attachment_count, arg_subpass.input_attachments);
      auto color_attachment_references = convert_to_vk_attachment_reference(arg_subpass.color_attachment_count, arg_subpass.color_attachments);
      auto resolve_attachment_references = convert_to_vk_attachment_reference(arg_subpass.color_attachment_count, arg_subpass.resolve_attachments);

      const auto& arg_depth_attachment_reference = _attachment_reference_depot.get(arg_subpass.depth_stencil_sttachment);

      if (attachment_description_index.find(arg_depth_attachment_reference.attachment) == attachment_description_index.end()) {
        auto index_pair = std::pair<std::string, uint32_t>(arg_depth_attachment_reference.attachment, static_cast<uint32_t>(attachment_description_index.size()));
        attachment_description_index.insert(index_pair);
      }
      auto depth_attachment_reference = std::make_shared<VkAttachmentReference>(
        VkAttachmentReference{
          attachment_description_index[arg_depth_attachment_reference.attachment],
          arg_depth_attachment_reference.layout
        }
      );

      tmp_input_attachment_references.push_back(input_attachment_references);
      tmp_color_attachment_references.push_back(color_attachment_references);
      tmp_resolve_attachment_references.push_back(resolve_attachment_references);
      tmp_depth_attachment_references.push_back(depth_attachment_reference);

      VkSubpassDescription subpass_description = {};
      subpass_description.inputAttachmentCount = static_cast<uint32_t>(input_attachment_references->size());
      subpass_description.pInputAttachments = input_attachment_references->data();
      subpass_description.colorAttachmentCount = static_cast<uint32_t>(color_attachment_references->size());
      subpass_description.pColorAttachments = color_attachment_references->data();
      subpass_description.pResolveAttachments = resolve_attachment_references->data();
      subpass_description.pDepthStencilAttachment = depth_attachment_reference.get();
      subpass_description.preserveAttachmentCount = arg_subpass.preserve_attachment_count;
      subpass_description.pPreserveAttachments = arg_subpass.preserve_attachments;

      return subpass_description;
    };

    std::vector<VkSubpassDescription> subpass_descriptions;
    subpass_descriptions.reserve(subpass_description_names.size());
    for (const auto& it : subpass_description_names) {
      subpass_descriptions.push_back(create_vk_subpass_description(it));
    }

    std::vector<VkAttachmentDescription> attachment_descriptions(attachment_description_index.size());
    for (const auto& it : attachment_description_index) {
      attachment_descriptions[it.second] = _attachment_description_depot.get(it.first);
    }

    VkRenderPassCreateInfo render_pass_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_descriptions.size());
    render_pass_info.pAttachments = attachment_descriptions.data();
    render_pass_info.subpassCount = static_cast<uint32_t>(subpass_descriptions.size());
    render_pass_info.pSubpasses = subpass_descriptions.data();
    render_pass_info.dependencyCount = 0;
    render_pass_info.pDependencies = nullptr;

    auto vk_render_pass = _createVkRenderPass(_parent->_vk_device, render_pass_info);
    return std::make_shared<RenderPassObject>(vk_render_pass);
  }

  void _destroyCore(std::shared_ptr<RenderPassObject> object) {
    _destroyVkRenderPass(_parent->_vk_device, object->_vk_render_pass);
  }

  AttachmentDescriptionDepot _attachment_description_depot;
  AttachmentReferenceDepot _attachment_reference_depot;
  SubpassDescriptionDepot _subpass_description_depot;
};

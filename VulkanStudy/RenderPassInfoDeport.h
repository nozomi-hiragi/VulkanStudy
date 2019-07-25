#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "AbstractDepot.h"

struct AttachmentReference {
  std::string      attachment;
  VkImageLayout    layout;
};

struct SubpassDescription {
  uint32_t                  input_attachment_count;
  const char*const*         input_attachments;
  uint32_t                  color_attachment_count;
  const char*const*         color_attachments;
  const char*const*         resolve_attachments;
  const char*               depth_stencil_sttachment;
  uint32_t                  preserve_attachment_count;
  const uint32_t*           preserve_attachments;
};

class AttachmentDescriptionDepot : public AbstractDepot<VkAttachmentDescription> {
public:
  void add(const std::string name,
    const VkFormat format,
    const VkSampleCountFlagBits samples,
    const VkAttachmentLoadOp load_op,
    const VkAttachmentStoreOp store_op,
    const VkAttachmentLoadOp stencil_load_op,
    const VkAttachmentStoreOp stencil_store_op,
    const VkImageLayout initial_layout,
    const VkImageLayout final_layout) {

    VkAttachmentDescription attachment_description = {};
    attachment_description.format = format;
    attachment_description.samples = samples;
    attachment_description.loadOp = load_op;
    attachment_description.storeOp = store_op;
    attachment_description.stencilLoadOp = stencil_load_op;
    attachment_description.stencilStoreOp = stencil_store_op;
    attachment_description.initialLayout = initial_layout;
    attachment_description.finalLayout = final_layout;

    AbstractDepot::add(name, attachment_description);
  }

protected:
private:
};

class AttachmentReferenceDepot : public AbstractDepot<AttachmentReference> {
public:

  void add(const std::string name,
    const std::string attachment,
    const VkImageLayout layout) {

    AttachmentReference attachment_reference = {};
    attachment_reference.attachment = attachment;
    attachment_reference.layout = layout;

    AbstractDepot::add(name, attachment_reference);
  }

protected:
private:
};


class SubpassDescriptionDepot : public AbstractDepot<SubpassDescription> {
public:
  void add(const std::string name,
    const uint32_t          input_attachment_count,
    const char*const*const  input_attachments,
    const uint32_t          color_attachment_count,
    const char*const*const  color_attachments,
    const char*const*const  resolve_attachments,
    const char*const        depth_stencil_sttachment,
    const uint32_t          preserve_attachment_count,
    const uint32_t*const    preserve_attachments
  ) {

    SubpassDescription subpass_description = {};
    subpass_description.input_attachment_count = input_attachment_count;
    subpass_description.input_attachments = input_attachments;
    subpass_description.color_attachment_count = color_attachment_count;
    subpass_description.color_attachments = color_attachments;
    subpass_description.resolve_attachments = resolve_attachments;
    subpass_description.depth_stencil_sttachment = depth_stencil_sttachment;
    subpass_description.preserve_attachment_count = preserve_attachment_count;
    subpass_description.preserve_attachments = preserve_attachments;

    AbstractDepot::add(name, subpass_description);
  }

protected:
private:
};

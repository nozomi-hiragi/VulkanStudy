#include "CommandBufferObject.h"

#include "ImageObject.h"
#include "ConstantBufferLayoutObject.h"

void CommandBufferObject::pipelineImageMemoryBarrier(VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage, std::shared_ptr<ImageObject> image, VkAccessFlags src_access, VkAccessFlags dst_access, VkImageLayout new_layout) {
  VkImageMemoryBarrier image_memory_barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
  image_memory_barrier.srcAccessMask = src_access;
  image_memory_barrier.dstAccessMask = dst_access;
  image_memory_barrier.oldLayout = image->_vk_layout;
  image_memory_barrier.newLayout = new_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = image->_vk_image;

  image_memory_barrier.subresourceRange.aspectMask = image->_vk_image_aspect_flags;
  image_memory_barrier.subresourceRange.baseMipLevel = 0;
  image_memory_barrier.subresourceRange.levelCount = image->_mip_level;
  image_memory_barrier.subresourceRange.baseArrayLayer = 0;
  image_memory_barrier.subresourceRange.layerCount = 1;

  vkCmdPipelineBarrier(_vk_command_buffer,
    src_stage,
    dst_stage,
    0,
    0, nullptr,
    0, nullptr,
    1, &image_memory_barrier);

  image->_vk_layout = image_memory_barrier.newLayout;
}

void CommandBufferObject::bindDescriptorSets(VkPipelineBindPoint bind_point, std::shared_ptr<ConstantBufferLayoutObject> constant_buffer_layout, uint32_t dynamic_offset_count, const uint32_t* dynamic_offsets) {
  vkCmdBindDescriptorSets(_vk_command_buffer, bind_point, constant_buffer_layout->_pipeline_layout->_vk_pipeline_layout, 0, 1, &constant_buffer_layout->_descriptor_set->_vk_descriptor_set, dynamic_offset_count, dynamic_offsets);
}

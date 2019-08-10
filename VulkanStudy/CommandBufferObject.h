#pragma once

#include <vulkan/vulkan.hpp>

#include "RenderPassObject.h"
#include "FramebufferObject.h"
#include "PipelineObject.h"

class ImageObject;

class CommandBufferObject {
public:
  CommandBufferObject(const VkCommandBuffer command_buffer):
    _vk_command_buffer(command_buffer),
    _viewport({}),
    _scissor({}) {
  }

  ~CommandBufferObject() {
  }

  void begin(const VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = flags;
    vkBeginCommandBuffer(_vk_command_buffer, &begin_info);
  }

  void end() {
    vkEndCommandBuffer(_vk_command_buffer);
  }

  void pipelineImageMemoryBarrier(VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage, std::shared_ptr<ImageObject> image, VkAccessFlags src_access, VkAccessFlags dst_access, VkImageLayout new_layout);

  void beginRenderPass(const std::shared_ptr<RenderPassObject> render_pass, const std::shared_ptr<FramebufferObject> framebuffer, const VkSubpassContents contents) {
    VkRenderPassBeginInfo render_pass_begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    render_pass_begin_info.renderPass = render_pass->_vk_render_pass;
    render_pass_begin_info.framebuffer = framebuffer->_vk_framebuffer;
    render_pass_begin_info.renderArea = _scissor;
    render_pass_begin_info.clearValueCount = static_cast<uint32_t>(_vk_clear_values.size());
    render_pass_begin_info.pClearValues = _vk_clear_values.data();

    vkCmdBeginRenderPass(_vk_command_buffer, &render_pass_begin_info, contents);
  }

  void endRenderPass() {
    vkCmdEndRenderPass(_vk_command_buffer);
  }

  void resetCommandBuffer(VkCommandBufferResetFlags flag) {
    vkResetCommandBuffer(_vk_command_buffer, flag);
  }

  void bindPipeline(const VkPipelineBindPoint bind_point, const std::shared_ptr<PipelineObject> pipeline) {
    vkCmdBindPipeline(_vk_command_buffer, bind_point, pipeline->_vk_pipeline);
  }

  void bindDescriptorSets(VkPipelineBindPoint bind_point, VkPipelineLayout layout, uint32_t first, uint32_t count, const VkDescriptorSet* descriptor_sets, uint32_t dynamic_offset_count, const uint32_t* dynamic_offsets) {
    vkCmdBindDescriptorSets(_vk_command_buffer, bind_point, layout, first, count, descriptor_sets, dynamic_offset_count, dynamic_offsets);
  }

  void setViewport(uint32_t first, uint32_t count, const VkViewport* viewports) {
    vkCmdSetViewport(_vk_command_buffer,first, count, viewports);
  }

  void bindVertexBuffers(uint32_t first, const uint32_t count, const VkBuffer* buffers, const VkDeviceSize* offset) {
    vkCmdBindVertexBuffers(_vk_command_buffer, first, count, buffers, offset);
  }

  void bindIndexBuffer(const VkBuffer buffer, const VkDeviceSize offset) {
    vkCmdBindIndexBuffer(_vk_command_buffer, buffer, offset, VK_INDEX_TYPE_UINT16);
  }

  void setScissor(uint32_t first, const VkRect2D& scissor) {
    vkCmdSetScissor(_vk_command_buffer, first, 1, &scissor);
  }

  void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
    vkCmdDraw(_vk_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
  }

  void drawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance) {
    vkCmdDrawIndexed(_vk_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
  }

  void setViewport(const float width, const float height) {
    _viewport.width = width;
    _viewport.height = height;
    _viewport.minDepth = 0;
    _viewport.maxDepth = 1;
  }

  void setScissor(const uint32_t width, const uint32_t height) {
    _scissor.extent.width = width;
    _scissor.extent.height = height;
  }

  void setViewSize(const uint32_t width, const uint32_t height) {
    setViewport(static_cast<float>(width), static_cast<float>(height));
    setScissor(width, height);
  }

  void applyViewSize() {
    setViewport(0, 1, &_viewport);
    setScissor(0, _scissor);
  }

  void setClearValue(const uint32_t index, const VkClearValue& clear_value) {
    if (_vk_clear_values.size() <= index) {
      _vk_clear_values.resize(index + 1);
    }
    _vk_clear_values[index] = clear_value;
  }

  void setClearColorValue(const uint32_t index, const VkClearColorValue& clear_color_value) {
    if (_vk_clear_values.size() <= index) {
      _vk_clear_values.resize(index + 1);
    }
    _vk_clear_values[index].color = clear_color_value;
  }

  void setClearDepthStencilValue(const uint32_t index, const VkClearDepthStencilValue& clear_depth_stencil_value) {
    if (_vk_clear_values.size() <= index) {
      _vk_clear_values.resize(index + 1);
    }
    _vk_clear_values[index].depthStencil = clear_depth_stencil_value;
  }

  const VkCommandBuffer _vk_command_buffer;
  VkViewport _viewport;
  VkRect2D _scissor;
  std::vector<VkClearValue> _vk_clear_values;
protected:
private:
};

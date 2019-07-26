#pragma once

#include <vulkan/vulkan.hpp>

class CommandBufferObject {
public:
  CommandBufferObject(const VkCommandBuffer command_buffer):
    _vk_command_buffer(command_buffer) {
  }

  ~CommandBufferObject() {
  }

  void begin() {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    vkBeginCommandBuffer(_vk_command_buffer, &begin_info);
  }

  void end() {
    vkEndCommandBuffer(_vk_command_buffer);
  }

  void beginRenderPass(const VkRenderPassBeginInfo& render_pass_info, const VkSubpassContents contents) {
    vkCmdBeginRenderPass(_vk_command_buffer, &render_pass_info, contents);
  }

  void endRenderPass() {
    vkCmdEndRenderPass(_vk_command_buffer);
  }

  void bindPipeline(const VkPipelineBindPoint bind_point, const VkPipeline pipeline) {
    vkCmdBindPipeline(_vk_command_buffer, bind_point, pipeline);
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

  void setScissor(uint32_t first, const VkRect2D& scissor) {
    vkCmdSetScissor(_vk_command_buffer, first, 1, &scissor);
  }

  void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
    vkCmdDraw(_vk_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
  }

  const VkCommandBuffer _vk_command_buffer;
protected:
private:
};

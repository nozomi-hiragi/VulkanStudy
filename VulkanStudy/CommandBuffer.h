#pragma once

#include <vulkan/vulkan.hpp>

class CommandBuffer {
public:
  CommandBuffer(): _command_buffer(nullptr) {
  }

  CommandBuffer(const vk::CommandBuffer command_buffer) : _command_buffer(command_buffer) {
  }

  ~CommandBuffer() {
  }

  vk::CommandBuffer getVkCommandBuffer() {
    return _command_buffer;
  }

  void setVkCommandBuffer(vk::CommandBuffer command_buffer) {
    _command_buffer = command_buffer;
  }

  void begin() {
    _command_buffer.begin(vk::CommandBufferBeginInfo()
      .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
  }

  void end() {
    _command_buffer.end();
  }

  void beginRenderPass(const vk::RenderPassBeginInfo& info, const vk::SubpassContents subpass) {
    _command_buffer.beginRenderPass(info, subpass);
  }

  void endRenderPass() {
    _command_buffer.endRenderPass();
  }

  void bindPipeline(const vk::PipelineBindPoint bind_point, const vk::Pipeline pipeline) {
    _command_buffer.bindPipeline(bind_point, pipeline);
  }

  void bindDescriptorSets(vk::PipelineBindPoint bind_point, vk::PipelineLayout layout, uint32_t first, uint32_t count, const vk::DescriptorSet* descriptor_sets, uint32_t dynamic_offset_count, const uint32_t* dynamic_offsets) {
    _command_buffer.bindDescriptorSets(bind_point, layout, first, count, descriptor_sets, dynamic_offset_count, dynamic_offsets);
  }

  void setViewport(uint32_t first, uint32_t count, const vk::Viewport* viewports) {
    _command_buffer.setViewport(first, count, viewports);
  }

  void bindVertexBuffers(uint32_t first, const vk::Buffer buffer, const vk::DeviceSize offset) {
    _command_buffer.bindVertexBuffers(first, buffer, offset);
  }

  void setScissor(uint32_t first, const vk::Rect2D& scissor) {
    _command_buffer.setScissor(first, scissor);
  }

  void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
    _command_buffer.draw(vertex_count, instance_count, first_vertex, first_instance);
  }

  vk::CommandBuffer aaa() {
    return _command_buffer;
  }

protected:
private:
  vk::CommandBuffer _command_buffer;
};

#pragma once

#include <vulkan/vulkan.hpp>

#include "Queue.h"
#include "CommandPool.h"
#include "CommandBuffer.h"

class PhysicalDeviceObject;

class Device {
public:
  static Device createDevice(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface);

  Device(): _device(nullptr) {
  }

  Device(const vk::Device device, const uint32_t present_queue_family_index) :
    _device(device),
    _present_queue_family_index(present_queue_family_index) {
  }

  ~Device() {
  }

  void destroy() {
    if (!_device) { return; }
    _device.waitIdle();
    _device.destroy();
    _device = nullptr;
  }

  // for queue
  Queue getPresentQueue(const uint32_t queue_index) {
    return Queue(_device.getQueue(_present_queue_family_index, queue_index));
  }

  // for command buffer
  CommandPool createPresentQueueCommandPool() {
    return CommandPool(_device.createCommandPool(vk::CommandPoolCreateInfo()
      .setQueueFamilyIndex(_present_queue_family_index)
      .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)));
  }

  void destroyCommandPool(CommandPool& command_pool) {
    auto vk_command_pool = command_pool.getVkCommandPool();
    if (!vk_command_pool) { return; }
    _device.destroyCommandPool(vk_command_pool);
    command_pool.setVkCommandPool(nullptr);
  }

  CommandBuffer allocateCommandBuffer(CommandPool& command_pool) {
    return CommandBuffer(_device.allocateCommandBuffers(vk::CommandBufferAllocateInfo()
      .setCommandPool(command_pool.getVkCommandPool())
      .setLevel(vk::CommandBufferLevel::ePrimary)
      .setCommandBufferCount(1))[0]);
  }

  void freeCommandBuffers(CommandPool& command_pool, CommandBuffer command_buffer) {
    auto vk_command_buffer = command_buffer.getVkCommandBuffer();
    if (!vk_command_buffer) { return; }
    _device.freeCommandBuffers(command_pool.getVkCommandPool(), vk_command_buffer);
    command_buffer.setVkCommandBuffer(nullptr);
  }

  // for fence
  void resetFence(const vk::Fence fence) {
    _device.resetFences(fence);
  }

  // for render pass
  vk::RenderPass createRenderPass(const vk::RenderPassCreateInfo& render_pass_info) {
    return _device.createRenderPass(render_pass_info);
  }

  void destroyRenderPass(const vk::RenderPass render_pass) {
    _device.destroyRenderPass(render_pass);
  }

  // for frame buffer
  vk::Framebuffer createFramebuffer(const vk::FramebufferCreateInfo& frame_buffer_info) {
    return _device.createFramebuffer(frame_buffer_info);
  }

  void destroyFramebuffer(const vk::Framebuffer frame_buffer) {
    _device.destroyFramebuffer(frame_buffer);
  }

  // for shader
  vk::ShaderModule createShaderModule(const vk::ShaderModuleCreateInfo& shader_module_info) {
    return _device.createShaderModule(shader_module_info);
  }

  void destroyShaderModule(const vk::ShaderModule shader) {
    _device.destroyShaderModule(shader);
  }

  // for pipeline
  vk::Pipeline createGraphicsPipeline(const vk::PipelineCache pipeline_cache, const vk::GraphicsPipelineCreateInfo& pipeline_info) {
    return _device.createGraphicsPipeline(pipeline_cache, pipeline_info);
  }

  void destroyPipeline(const vk::Pipeline pipeline) {
    _device.destroyPipeline(pipeline);
  }

  vk::PipelineLayout createPipelineLayout(const vk::PipelineLayoutCreateInfo& pipeline_layout_info) {
    return _device.createPipelineLayout(pipeline_layout_info);
  }

  void destroyPipelineLayout(const vk::PipelineLayout pipeline_layout) {
    _device.destroyPipelineLayout(pipeline_layout);
  }

  // fom memory
  void* mapMemory(const vk::DeviceMemory memory, const uint64_t offset, const uint64_t size) {
    return _device.mapMemory(memory, offset, size);
  }

  void unmapMemory(const vk::DeviceMemory memory) {
    _device.unmapMemory(memory);
  }

  // for buffer
  void bindBufferMemory(const vk::Buffer buffer, const vk::DeviceMemory memory, const uint64_t offset) {
    _device.bindBufferMemory(buffer, memory, offset);
  }

  // for image
  void bindImageMemory(const vk::Image image, const vk::DeviceMemory memory, const uint64_t offset) {
    _device.bindImageMemory(image, memory, offset);
  }

  // for descriptor
  vk::DescriptorSetLayout createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& descriptot_set_layout_info) {
    return _device.createDescriptorSetLayout(descriptot_set_layout_info);
  }

  void destroyDescriptorSetLayout(const vk::DescriptorSetLayout descriptor_set_layout) {
    _device.destroyDescriptorSetLayout(descriptor_set_layout);
  }

  vk::DescriptorPool createDescriptorPool(const vk::DescriptorPoolCreateInfo& descriptor_pool_info) {
    return _device.createDescriptorPool(descriptor_pool_info);
  }

  void destroyDescriptorPool(const  vk::DescriptorPool descriptor_pool) {
    _device.destroyDescriptorPool(descriptor_pool);
  }

  std::vector<vk::DescriptorSet> allocateDescriptorSets(const vk::DescriptorSetAllocateInfo& descriptor_set_info) {
    return _device.allocateDescriptorSets(descriptor_set_info);
  }

  void updateDescriptorSets(const uint32_t write_count, const vk::WriteDescriptorSet* writes, const uint32_t copy_count, const vk::CopyDescriptorSet* copies) {
    _device.updateDescriptorSets(write_count, writes, copy_count, copies);
  }

  // for swapchain
  void acquireNextImage(VkSwapchainKHR swapchain, const uint64_t timeout, const vk::Semaphore semaphore, const vk::Fence fence, uint32_t* image_index) {
    _device.acquireNextImageKHR(swapchain, timeout, semaphore, fence, image_index);
  }

  auto getVkDevice() {
    return _device;
  }

protected:
private:
  vk::Device _device;
  uint32_t _present_queue_family_index;
};

#pragma once

#include <vulkan/vulkan.hpp>

#include "Queue.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Swapchain.h"

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
  vk::Fence createFence(const vk::FenceCreateInfo& fence_info) {
    return _device.createFence(fence_info);
  }

  void destroyFence(const vk::Fence fence) {
    _device.destroyFence(fence);
  }

  vk::Result waitForFences(const uint32_t count, const vk::Fence* fences, const bool wait_all, const uint64_t timeout) {
    return _device.waitForFences(count, fences, wait_all ? VK_TRUE : VK_FALSE, timeout);
  }

  void resetFence(const vk::Fence fence) {
    _device.resetFences(fence);
  }

  // for semaphoer
  vk::Semaphore createSemaphore(const vk::SemaphoreCreateInfo& semaphore_info) {
    return _device.createSemaphore(semaphore_info);
  }

  void destroySemaphore(const vk::Semaphore semaphore) {
    _device.destroySemaphore(semaphore);
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
  vk::DeviceMemory allocateMemory(const vk::MemoryAllocateInfo& memory_allocate_info) {
    return _device.allocateMemory(memory_allocate_info);
  }

  void freeMemory(const vk::DeviceMemory memory) {
    _device.freeMemory(memory);
  }

  void* mapMemory(const vk::DeviceMemory memory, const uint64_t offset, const uint64_t size) {
    return _device.mapMemory(memory, offset, size);
  }

  void unmapMemory(const vk::DeviceMemory memory) {
    _device.unmapMemory(memory);
  }

  // for buffer
  vk::Buffer createBuffer(const vk::BufferCreateInfo& buffer_info) {
    return _device.createBuffer(buffer_info);
  }

  void destroyBuffer(const vk::Buffer buffer) {
    _device.destroyBuffer(buffer);
  }

  vk::MemoryRequirements getBufferMemoryRequirements(const vk::Buffer buffer) {
    return _device.getBufferMemoryRequirements(buffer);
  }

  void bindBufferMemory(const vk::Buffer buffer, const vk::DeviceMemory memory, const uint64_t offset) {
    _device.bindBufferMemory(buffer, memory, offset);
  }

  // for image
  vk::Image createImage(const vk::ImageCreateInfo& image_info) {
    return _device.createImage(image_info);
  }

  void destroyImage(const vk::Image image) {
    _device.destroyImage(image);
  }

  vk::ImageView createImageView(const vk::ImageViewCreateInfo& image_view_info) {
    return _device.createImageView(image_view_info);
  }

  void destroyImageView(const vk::ImageView image_view) {
    _device.destroyImageView(image_view);
  }

  vk::MemoryRequirements getImageMemoryRequirements(const vk::Image image) {
    return _device.getImageMemoryRequirements(image);
  }

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
  static VkSurfaceFormatKHR getFixSurfaceProperties(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    uint32_t count = 0;
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(count);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, surface_formats.data());

    auto surface_format = surface_formats[0];
    if (surface_formats.size() == 1 && surface_format.format == VK_FORMAT_UNDEFINED) {
      surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    return surface_format;
  }

  Swapchain createSwapchain(VkSurfaceKHR surface, VkPhysicalDevice physical_device, const uint32_t width, const uint32_t height) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

    auto surface_format = getFixSurfaceProperties(physical_device, surface);

    VkSurfaceTransformFlagBitsKHR pre_transform;
    if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
      pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else {
      pre_transform = surface_capabilities.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR composite_alpha_flags[] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (auto it : composite_alpha_flags) {
      if (surface_capabilities.supportedCompositeAlpha & it) {
        composite_alpha = it;
        break;
      }
    }

    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = surface;
    swapchain_info.minImageCount = surface_capabilities.minImageCount;
    swapchain_info.imageFormat = surface_format.format;
    swapchain_info.imageColorSpace = surface_format.colorSpace;
    swapchain_info.imageExtent = { width, height };
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_info.queueFamilyIndexCount = 0; // Ç†ÇÍÅH
    swapchain_info.pQueueFamilyIndices = nullptr;
    swapchain_info.preTransform = pre_transform;
    swapchain_info.compositeAlpha = composite_alpha;
    swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_info.clipped = true;
    swapchain_info.oldSwapchain = nullptr;

    return Swapchain(_device.createSwapchainKHR(swapchain_info), surface_format);
  }

  void destroySwapchain(Swapchain swapchain) {
    auto vk_swapchain = swapchain.getVkSwapchain();
    if (!vk_swapchain) { return; }
    _device.destroySwapchainKHR(vk_swapchain);
    swapchain.setVkSwapchain(nullptr);
  }

  std::vector<vk::Image> getSwapchainImages(Swapchain swapchain) {
    return _device.getSwapchainImagesKHR(swapchain.getVkSwapchain());
  }

  void acquireNextImage(Swapchain swapchain, const uint64_t timeout, const vk::Semaphore semaphore, const vk::Fence fence, uint32_t* image_index) {
    _device.acquireNextImageKHR(swapchain.getVkSwapchain(), timeout, semaphore, fence, image_index);
  }

protected:
private:
  vk::Device _device;
  uint32_t _present_queue_family_index;
};

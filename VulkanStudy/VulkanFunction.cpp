
#include "VulkanFunction.h"
#include <vector>
#include <fstream>

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "InstanceFactory.h"
#include "SurfaceFactory.h"
#include "SwapchainFactory.h"
#include "ImageFactory.h"
#include "ImageViewFactory.h"
#include "DeviceMemoryFactory.h"
#include "BufferFactory.h"
#include "FenceFactory.h"
#include "SemaphoreFactory.h"
#include "CommandPoolFactory.h"
#include "CommandBufferObject.h"
#include "ShaderModuleFactory.h"
#include "RenderPassFactory.h"
#include "FramebufferFactory.h"

#include "Renderer.h"
#include "Device.h"
#include "QueueObject.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;
constexpr auto PRIMALY_PHYSICAL_DEVICE_INDEX = 0;

InstanceFactory _instance_factory;
SurfaceFactory _surface_factory;
SwapchainFactory _swapchain_factory;
ImageFactory _image_factory;
ImageViewFactory _image_view_factory;
DeviceMemoryFactory _device_memory_factory;
BufferFactory _buffer_factory;
FenceFactory _fence_factory;
SemaphoreFactory _semaphore_factory;
CommandPoolFactory _command_pool_factory;
ShaderModuleFactory _shader_module_factory;
RenderPassFactory _render_pass_factory;
FramebufferFactory _framebuffer_factory;

std::shared_ptr<InstanceObject> _instance_object;
std::shared_ptr<PhysicalDeviceObject> _physical_device_object;
std::shared_ptr<SurfaceObject> _surface_object;
std::shared_ptr<SwapchainObject> _swapchain;
std::shared_ptr<ImageObject> _depth_image;
std::vector<std::shared_ptr<ImageViewObject>> _swapchain_image_views;
std::shared_ptr<ImageViewObject> _depth_image_view;
std::shared_ptr<DeviceMemoryObject> _depth_memory;
std::shared_ptr<DeviceMemoryObject> _uniform_memory;
std::shared_ptr<DeviceMemoryObject> _vertex_memory;
std::shared_ptr<BufferObject> _uniform_buffer;
std::shared_ptr<BufferObject> _vertex_buffer;
std::shared_ptr<FenceObject> _fence;
std::shared_ptr<SemaphoreObject> _image_semaphore;
std::shared_ptr<QueueObject> _queue;
std::shared_ptr<CommandPoolObject> _command_pool;
std::shared_ptr<CommandBufferObject> _command_buffer;
std::shared_ptr<ShaderModuleObject> _vs;
std::shared_ptr<ShaderModuleObject> _ps;
std::shared_ptr<RenderPassObject> _render_pass;
std::vector<std::shared_ptr<FramebufferObject>> _framebuffers;

Device _device;
vk::DescriptorSetLayout g_descriptor_set_layout = nullptr;
vk::PipelineLayout g_pipeline_layout = nullptr;
vk::DescriptorPool g_descriptor_pool = nullptr;
VkDescriptorSet g_descriptor_set = nullptr;
vk::Pipeline g_pipeline = nullptr;

uint32_t g_current_buffer = 0;

glm::mat4 g_mvp;

struct Vertex {
  glm::vec4 pos;
  glm::vec4 col;
};

Vertex poly[] = {
  { glm::vec4(-1, +1, 1, 1), glm::vec4(1, 0, 0, 0) },
  { glm::vec4(+1, +1, 1, 1), glm::vec4(0, 1, 0, 0) },
  { glm::vec4(-1, -1, 1, 1), glm::vec4(0, 0, 1, 0) },

  { glm::vec4(-1, +10, 1, 1), glm::vec4(1, 0, 0, 0) },
  { glm::vec4(-1, -1, 1, 1), glm::vec4(0, 1, 0, 0) },
  { glm::vec4(+1, -1, 1, 1), glm::vec4(0, 0, 1, 0) },
};

uint32_t _width;
uint32_t _height;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

void initVulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height) {
  _width = width;
  _height = height;

  // init
  _instance_object = _instance_factory.createObject(nullptr, APP_NAME, APP_VERSION);

  _physical_device_object = _instance_object->_physical_devices[PRIMALY_PHYSICAL_DEVICE_INDEX];

  // Create Surface
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  _surface_object = _surface_factory.createObject(_instance_object, hinstance, hwnd);
#endif

  // Create Device
  _device = Device::createDevice(_physical_device_object, _surface_object->_vk_surface);

  // Get queue
  _queue = _device.getQueue();

  // Create Command pool
  _command_pool = _command_pool_factory.createCommandPool(_device.getVkDevice(), _queue->_vk_queue_family_index);

  // Create primaly Command buffer
  _command_buffer = _command_pool->allocateCommandBuffer(_device.getVkDevice());

  // Create Swapchain
  _swapchain = _swapchain_factory.createSwapchain(_device.getVkDevice(), _surface_object->_vk_surface, _physical_device_object->_physical_device, width, height);

  // Create Swapchain image
  _swapchain_image_views.reserve(_swapchain->_swapchain_image_count);

  // Create Swapchain image view
  for (auto image : _swapchain->_swapchain_images) {
    VkImageSubresourceRange subresource_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    _swapchain_image_views.push_back(_image_view_factory.createImageView(_device.getVkDevice(), image, subresource_range));
  }

  // Create Depth image
  _depth_image = _image_factory.createImage(
    _device.getVkDevice(),
    VK_FORMAT_D16_UNORM,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    width,
    height);

  // Allocate depth memory
  {
    auto memory_type_index = _physical_device_object->findProperties(_depth_image->_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    _depth_memory = _device_memory_factory.createDeviceMemory(_device.getVkDevice(), _depth_image->_memory_requirements.size, memory_type_index);
  }

  // Bind memory to depth image
  _device.bindImageMemory(_depth_image->_vk_image, _depth_memory->_vk_device_memory, 0);

  // Create depth image view
  {
    VkImageSubresourceRange subresource_range = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
    _depth_image_view = _image_view_factory.createImageView(_device.getVkDevice(), _depth_image, subresource_range);
  }

  // Create matrix
  {
    projection = glm::perspectiveFov(glm::radians(45.f), static_cast<float>(width), static_cast<float>(height), 0.1f, 100.f);
    view = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
    model = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
    g_mvp = projection * view * model;
  }

  // Create uniform buffer
  _uniform_buffer = _buffer_factory.createBuffer(_device.getVkDevice(), sizeof(g_mvp), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  // Allocate uniform memory
  {
    auto memory_type_index = _physical_device_object->findProperties(_uniform_buffer->_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _uniform_memory = _device_memory_factory.createDeviceMemory(_device.getVkDevice(), _uniform_buffer->_memory_requirements.size, memory_type_index);
  }

  // Wrinte to memory
  {
    auto data = _device.mapMemory(_uniform_memory->_vk_device_memory, 0, _uniform_buffer->_memory_requirements.size);

    memcpy(data, &g_mvp, sizeof(g_mvp));

    _device.unmapMemory(_uniform_memory->_vk_device_memory);
  }

  // Bind memory to buffer
  {
    _device.bindBufferMemory(_uniform_buffer->_vk_buffer, _uniform_memory->_vk_device_memory, 0);
  }

  // Create descriptor set layout
  {
    auto descriptor_set_layout_binding = vk::DescriptorSetLayoutBinding()
      .setBinding(0)
      .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
      .setDescriptorCount(1)
      .setStageFlags(vk::ShaderStageFlagBits::eVertex)
      .setPImmutableSamplers(nullptr);

    auto descriptor_set_layout_info = vk::DescriptorSetLayoutCreateInfo()
      .setBindingCount(1)
      .setPBindings(&descriptor_set_layout_binding);

    g_descriptor_set_layout = _device.createDescriptorSetLayout(descriptor_set_layout_info);
  }

  // Create Pipeline layout
  {
    auto range = vk::PushConstantRange()
      .setStageFlags(vk::ShaderStageFlagBits::eVertex)
      .setOffset(0)
      .setSize(sizeof(g_mvp));

    auto pipeline_layout_info = vk::PipelineLayoutCreateInfo()
      .setPushConstantRangeCount(1)
      .setPPushConstantRanges(&range)
      .setSetLayoutCount(1)
      .setPSetLayouts(&g_descriptor_set_layout);

    g_pipeline_layout = _device.createPipelineLayout(pipeline_layout_info);
  }

  // Crearte descripter pool

  vk::DescriptorPoolSize pool_sizes[] = {
    vk::DescriptorPoolSize()
    .setType(vk::DescriptorType::eUniformBufferDynamic)
    .setDescriptorCount(1),
  };

  auto descriptor_pool_info = vk::DescriptorPoolCreateInfo()
    .setMaxSets(1)
    .setPoolSizeCount(1)
    .setPPoolSizes(pool_sizes);

  g_descriptor_pool = _device.createDescriptorPool(descriptor_pool_info);

  // Allocarte descriptor set

  auto descriptor_set_info = vk::DescriptorSetAllocateInfo()
    .setDescriptorPool(g_descriptor_pool)
    .setDescriptorSetCount(1)
    .setPSetLayouts(&g_descriptor_set_layout);

  g_descriptor_set = _device.allocateDescriptorSets(descriptor_set_info)[0];

  // Update descriptor sets

  auto descriptor_buffer_info = vk::DescriptorBufferInfo()
    .setOffset(0)
    .setRange(sizeof(g_mvp))//VK_WHOLE_SIZE??
    .setBuffer(_uniform_buffer->_vk_buffer);

  vk::WriteDescriptorSet write_descriptor_sets[] = {
    vk::WriteDescriptorSet()
    .setDescriptorCount(1)
    .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
    .setPBufferInfo(&descriptor_buffer_info)
    .setDstSet(g_descriptor_set)
  };

  _device.updateDescriptorSets(1, write_descriptor_sets, 0, nullptr);

  // Create render pass
  _render_pass = _render_pass_factory.createObject(_device.getVkDevice(), _swapchain->_vk_format, _depth_image->_vk_format);

  // Create shader module

  auto get_binary = [](const wchar_t* filename) {
    FILE* fp;
    if (_wfopen_s(&fp, filename, L"rb") != 0) throw std::runtime_error("File not found");

    fseek(fp, 0, SEEK_END);
    auto size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    auto buf = std::make_unique<unsigned char[]>(size);
    fread(buf.get(), sizeof(unsigned char), size, fp);
    fclose(fp);
    return std::pair<std::unique_ptr<unsigned char[]>, size_t>(std::move(buf), size);
  };

  auto vs_bin = get_binary(L"vspc.vert.spv");

  _vs = _shader_module_factory.createObject(_device.getVkDevice(), vs_bin.second, reinterpret_cast<uint32_t*>(vs_bin.first.get()));

  auto ps_bin = get_binary(L"ps.frag.spv");

  _ps = _shader_module_factory.createObject(_device.getVkDevice(), ps_bin.second, reinterpret_cast<uint32_t*>(ps_bin.first.get()));

  vk::PipelineShaderStageCreateInfo shader_stage_info[] = {
    vk::PipelineShaderStageCreateInfo()
    .setStage(vk::ShaderStageFlagBits::eVertex)
    .setModule(_vs->_vk_shader_module)
    .setPName("main"),

    vk::PipelineShaderStageCreateInfo()
    .setStage(vk::ShaderStageFlagBits::eFragment)
    .setModule(_ps->_vk_shader_module)
    .setPName("main")
  };

  //

  vk::ImageView attachments[2];
  attachments[1] = _depth_image_view->_vk_image_view;

  auto const frame_buffer_info = vk::FramebufferCreateInfo()
    .setRenderPass(_render_pass->_vk_render_pass)
    .setAttachmentCount(2)
    .setPAttachments(attachments)
    .setWidth(width)
    .setHeight(height)
    .setLayers(1);

  _framebuffers.reserve(_swapchain->_swapchain_image_count);

  for (uint32_t i = 0; i < _swapchain->_swapchain_image_count; i++) {
    attachments[0] = _swapchain_image_views[i]->_vk_image_view;
    _framebuffers.push_back(_framebuffer_factory.createObject(
      _device.getVkDevice(),
      _render_pass->_vk_render_pass,
      _swapchain_image_views[i]->_vk_image_view,
      _depth_image_view->_vk_image_view,
      width,
      height));
  }

  // Create Vertex buffer

  auto vertex_buffer_info = vk::BufferCreateInfo()
    .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
    .setSize(sizeof(poly))
    .setSharingMode(vk::SharingMode::eExclusive);

  _vertex_buffer = _buffer_factory.createBuffer(_device.getVkDevice(), sizeof(poly), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

  // Allocate vertex buffer memory

  auto memory_type_bits = _vertex_buffer->_memory_requirements.memoryTypeBits;
  auto memory_property_bits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  auto memory_type_index = _physical_device_object->findProperties(memory_type_bits, memory_property_bits);

  _vertex_memory = _device_memory_factory.createDeviceMemory(_device.getVkDevice(), _vertex_buffer->_memory_requirements.size, memory_type_index);

  // Store vertex buffer

  auto vertex_map = _device.mapMemory(_vertex_memory->_vk_device_memory, 0, _vertex_buffer->_memory_requirements.size);

  memcpy(vertex_map, &poly, sizeof(poly));

  _device.unmapMemory(_vertex_memory->_vk_device_memory);

  _device.bindBufferMemory(_vertex_buffer->_vk_buffer, _vertex_memory->_vk_device_memory, 0);

  // Description

  auto vertex_binding_description = vk::VertexInputBindingDescription()
    .setBinding(0)
    .setInputRate(vk::VertexInputRate::eVertex)
    .setStride(sizeof(Vertex));

  vk::VertexInputAttributeDescription vertex_attribute_descriptions[] = {
    vk::VertexInputAttributeDescription()
    .setBinding(0)
    .setLocation(0)
    .setFormat(vk::Format::eR32G32B32A32Sfloat)
    .setOffset(0),

    vk::VertexInputAttributeDescription()
    .setBinding(0)
    .setLocation(1)
    .setFormat(vk::Format::eR32G32B32A32Sfloat)
    .setOffset(16)
  };

  // Create semaphore
  _image_semaphore = _semaphore_factory.createSemaphore(_device.getVkDevice());

  _device.acquireNextImage(_swapchain->_vk_swapchain, UINT64_MAX, _image_semaphore->_vk_semaphore, nullptr, &g_current_buffer);

  // Create pipeline

  vk::DynamicState dynamic_states[2] = {
    vk::DynamicState::eViewport,
    vk::DynamicState::eScissor
  };

  auto dynamic_state_info = vk::PipelineDynamicStateCreateInfo()
    .setPDynamicStates(dynamic_states)
    .setDynamicStateCount(2);

  auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo()
    .setVertexBindingDescriptionCount(1)
    .setPVertexBindingDescriptions(&vertex_binding_description)
    .setVertexAttributeDescriptionCount(2)
    .setPVertexAttributeDescriptions(vertex_attribute_descriptions);

  auto input_assembly_info = vk::PipelineInputAssemblyStateCreateInfo()
    .setTopology(vk::PrimitiveTopology::eTriangleList);

  auto rasterization_info = vk::PipelineRasterizationStateCreateInfo()
    .setDepthClampEnable(VK_FALSE)
    .setRasterizerDiscardEnable(VK_FALSE)
    .setPolygonMode(vk::PolygonMode::eFill)
    .setCullMode(vk::CullModeFlagBits::eBack)
    .setFrontFace(vk::FrontFace::eClockwise)
    .setDepthBiasEnable(VK_FALSE)
    .setLineWidth(1.0f);

  vk::PipelineColorBlendAttachmentState color_blend_attachments[1] = {
    vk::PipelineColorBlendAttachmentState()
    .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA) };

  auto color_blend_info = vk::PipelineColorBlendStateCreateInfo()
    .setAttachmentCount(1)
    .setPAttachments(color_blend_attachments);

  auto viewport = vk::Viewport()
    .setWidth((float)width)
    .setHeight((float)height)
    .setMinDepth((float)0.0f)
    .setMaxDepth((float)1.0f);
  vk::Rect2D scissor(vk::Offset2D(0, 0), vk::Extent2D(width, height));
  auto viewport_info = vk::PipelineViewportStateCreateInfo()
    .setViewportCount(1)
    .setPViewports(&viewport)
    .setScissorCount(1)
    .setPScissors(&scissor);

  auto stencil_op = vk::StencilOpState()
    .setFailOp(vk::StencilOp::eKeep)
    .setPassOp(vk::StencilOp::eKeep)
    .setCompareOp(vk::CompareOp::eAlways);

  auto depth_stencil_info = vk::PipelineDepthStencilStateCreateInfo()
    .setDepthTestEnable(VK_TRUE)
    .setDepthWriteEnable(VK_TRUE)
    .setDepthCompareOp(vk::CompareOp::eLessOrEqual)
    .setDepthBoundsTestEnable(VK_FALSE)
    .setStencilTestEnable(VK_FALSE)
    .setFront(stencil_op)
    .setBack(stencil_op);

  auto multisample_info = vk::PipelineMultisampleStateCreateInfo()
    .setRasterizationSamples(vk::SampleCountFlagBits::e1);

  auto pipeline_info = vk::GraphicsPipelineCreateInfo()
    .setStageCount(2)
    .setPStages(shader_stage_info)
    .setPVertexInputState(&vertex_input_info)
    .setPInputAssemblyState(&input_assembly_info)
    .setPViewportState(&viewport_info)
    .setPRasterizationState(&rasterization_info)
    .setPMultisampleState(&multisample_info)
    .setPDepthStencilState(&depth_stencil_info)
    .setPColorBlendState(&color_blend_info)
    .setPDynamicState(nullptr/*&dynamic_state_info*/)
    .setLayout(g_pipeline_layout)
    .setRenderPass(_render_pass->_vk_render_pass);

  g_pipeline = _device.createGraphicsPipeline(nullptr, pipeline_info);

  // Begin command

  _command_buffer->begin();

  // Begin render pass

  vk::ClearValue clear_values[2] = {
    vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})),
    vk::ClearDepthStencilValue(1.0f, 0u)
  };

  auto render_begin_info = vk::RenderPassBeginInfo()
    .setRenderPass(_render_pass->_vk_render_pass)
    .setFramebuffer(_framebuffers[g_current_buffer]->_vk_framebuffer)
    .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(width, height)))
    .setClearValueCount(2)
    .setPClearValues(clear_values);

  _command_buffer->beginRenderPass(render_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  //

  _command_buffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline);

  //

  uint32_t ofst = 0;
  _command_buffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline_layout, 0, 1, &g_descriptor_set, 1, &ofst);

  //


  //myCommandBuffer.setViewport(0, 1, &viewport);

  // Bind vertex buffer

  vk::DeviceSize vertex_offset = 0;
  _command_buffer->bindVertexBuffers(0, _vertex_buffer->_vk_buffer, vertex_offset);

  //

  //myCommandBuffer.setScissor(0, scissor);

  //

  uint32_t vertex_count = 2 * 3;
  uint32_t instance_count = 1;
  uint32_t first_vertex = 0;
  uint32_t first_instance = 0;

  _command_buffer->draw(vertex_count, instance_count, first_vertex, first_instance);

  // End render pass

  _command_buffer->endRenderPass();

  // End command

  _command_buffer->end();

  //
  _fence = _fence_factory.createFence(_device.getVkDevice());

  //

  VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkCommandBuffer command_buffers[] = { _command_buffer->_vk_command_buffer };
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = command_buffers;

  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &_image_semaphore->_vk_semaphore;
  submit_info.pWaitDstStageMask = &pipe_stage_flags;


  vkQueueSubmit(_queue->_vk_queue, 1, &submit_info, _fence->_vk_fence);

  //

  VkResult res;

  do {
    res = FenceObject::vkWaitForFence(_device.getVkDevice(), _fence->_vk_fence, UINT64_MAX);
  } while (res == VK_TIMEOUT);

  //

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &_swapchain->_vk_swapchain;
  present_info.pImageIndices = &g_current_buffer;

  vkQueuePresentKHR(_queue->_vk_queue, &present_info);

}
float a = 0;
void updateVulkan() {


  _device.acquireNextImage(_swapchain->_vk_swapchain, UINT64_MAX, _image_semaphore->_vk_semaphore, nullptr, &g_current_buffer);

  FenceObject::vkWaitForFence(_device.getVkDevice(), _fence->_vk_fence, UINT64_MAX);
  _device.resetFence(_fence->_vk_fence);


  _command_buffer->begin();

  vk::ClearValue clear_values[2] = {
    vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})),
    vk::ClearDepthStencilValue(1.0f, 0u)
  };

  auto render_begin_info = vk::RenderPassBeginInfo()
    .setRenderPass(_render_pass->_vk_render_pass)
    .setFramebuffer(_framebuffers[g_current_buffer]->_vk_framebuffer)
    .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(_width, _height)))
    .setClearValueCount(2)
    .setPClearValues(clear_values);

  _command_buffer->beginRenderPass(render_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  // ~~~

  _command_buffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline);

  uint32_t ofst = 0;
  _command_buffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline_layout, 0, 1, &g_descriptor_set, 1, &ofst);

  auto viewport = vk::Viewport()
    .setWidth((float)_width)
    .setHeight((float)_height)
    .setMinDepth((float)0.0f)
    .setMaxDepth((float)1.0f);

  //myCommandBuffer.setViewport(0, 1, &viewport);

  vk::Rect2D scissor(vk::Offset2D(0, 0), vk::Extent2D(_width, _height));

  //myCommandBuffer.setScissor(0, scissor);
  
  a+=0.01f;
  model = glm::translate(glm::mat4(1), glm::vec3(a, 0, 0));
  g_mvp = projection * view * model;
 
  vkCmdPushConstants(_command_buffer->_vk_command_buffer, g_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(g_mvp), &g_mvp);

  vk::DeviceSize vertex_offset = 0;
  _command_buffer->bindVertexBuffers(0, _vertex_buffer->_vk_buffer, vertex_offset);


  uint32_t vertex_count = 2 * 3;
  uint32_t instance_count = 1;
  uint32_t first_vertex = 0;
  uint32_t first_instance = 0;

  _command_buffer->draw(vertex_count, instance_count, first_vertex, first_instance);
  // ~~~~

  _command_buffer->endRenderPass();

  _command_buffer->end();

  VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkCommandBuffer command_buffers[] = { _command_buffer->_vk_command_buffer };
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = command_buffers;

  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &_image_semaphore->_vk_semaphore;
  submit_info.pWaitDstStageMask = &pipe_stage_flags;
  vkQueueSubmit(_queue->_vk_queue, 1, &submit_info, _fence->_vk_fence);

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &_swapchain->_vk_swapchain;
  present_info.pImageIndices = &g_current_buffer;

  vkQueuePresentKHR(_queue->_vk_queue, &present_info);
}

void uninitVulkan() {

  _fence_factory.destroyFence(_device.getVkDevice(), _fence);

  if (g_pipeline) {
    _device.destroyPipeline(g_pipeline);
    g_pipeline = nullptr;
  }

  _semaphore_factory.destroySemaphore(_device.getVkDevice(), _image_semaphore);

  _device_memory_factory.destroyDeviceMemory(_device.getVkDevice(), _vertex_memory);

  _buffer_factory.destroyBuffer(_device.getVkDevice(), _vertex_buffer);

  for (auto& framebuffer : _framebuffers) {
    _framebuffer_factory.destroyObject(_device.getVkDevice(), framebuffer);
  }
  _framebuffers.clear();

  _shader_module_factory.destroyObject(_device.getVkDevice(), _ps);

  _shader_module_factory.destroyObject(_device.getVkDevice(), _vs);

  _render_pass_factory.destroyObject(_device.getVkDevice(), _render_pass);

  if (g_descriptor_pool) {
    _device.destroyDescriptorPool(g_descriptor_pool);
    g_descriptor_pool = nullptr;
  }

  if (g_pipeline_layout) {
    _device.destroyPipelineLayout(g_pipeline_layout);
    g_pipeline_layout = nullptr;
  }

  if (g_descriptor_set_layout) {
    _device.destroyDescriptorSetLayout(g_descriptor_set_layout);
    g_descriptor_set_layout = nullptr;
  }

  _device_memory_factory.destroyDeviceMemory(_device.getVkDevice(), _uniform_memory);

  _buffer_factory.destroyBuffer(_device.getVkDevice(), _uniform_buffer);

  _image_view_factory.destroyImageView(_device.getVkDevice(), _depth_image_view);

  _device_memory_factory.destroyDeviceMemory(_device.getVkDevice(), _depth_memory);

  _image_factory.destroyImage(_device.getVkDevice(), _depth_image);

  _command_pool->freeCommandBuffers(_device.getVkDevice(), _command_buffer);

  _command_pool_factory.destroyCommandPool(_device.getVkDevice(), _command_pool);

  for (auto& image_view : _swapchain_image_views) {
    _image_view_factory.destroyImageView(_device.getVkDevice(), image_view);
  } 
  _swapchain_image_views.clear();

  _swapchain_factory.destroySwapchain(_device.getVkDevice(), _swapchain);

  _device.destroy();

  _surface_factory.destroyObject(_surface_object);

  _instance_factory.destroyObject(_instance_object);
}

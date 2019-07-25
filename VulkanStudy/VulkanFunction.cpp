
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
#include "ShaderModuleFactory.h"
#include "RenderPassFactory.h"
#include "FramebufferFactory.h"
#include "DescriptorSetLayoutFactory.h"
#include "DescriptorPoolFactory.h"
#include "PipelineLayoutFactory.h"
#include "PipelineFactory.h"
#include "DeviceFactory.h"

#include "Renderer.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Renderer _renderer;

BufferFactory _buffer_factory;
FenceFactory _fence_factory;
SemaphoreFactory _semaphore_factory;
ShaderModuleFactory _shader_module_factory;
RenderPassFactory _render_pass_factory;
FramebufferFactory _framebuffer_factory;
DescriptorSetLayoutFactory _descriptor_set_layout_factory;
DescriptorPoolFactory _descriptor_pool_factory;
PipelineLayoutFactory _pipeline_layout_factory;
PipelineFactory _pipeline_factory;

std::shared_ptr<DeviceMemoryObject> _uniform_memory;
std::shared_ptr<DeviceMemoryObject> _vertex_memory;
std::shared_ptr<BufferObject> _uniform_buffer;
std::shared_ptr<BufferObject> _vertex_buffer;
std::shared_ptr<FenceObject> _fence;
std::shared_ptr<SemaphoreObject> _image_semaphore;
std::shared_ptr<ShaderModuleObject> _vs;
std::shared_ptr<ShaderModuleObject> _ps;
std::shared_ptr<RenderPassObject> _render_pass;
std::vector<std::shared_ptr<FramebufferObject>> _framebuffers;
std::shared_ptr<DescriptorSetLayoutObject> _descriptor_set_layout;
std::shared_ptr<DescriptorPoolObject> _descriptor_pool;
std::shared_ptr<PipelineLayoutObject> _pipeline_layout;
std::shared_ptr<DescriptorSetObject> _descriptor_set;
std::shared_ptr<PipelineObject> _pipeline;

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

  _renderer.init(APP_NAME, APP_VERSION, width, height, hinstance, hwnd);

  // Uniform buffer
  {
    // Create matrix
    {
      projection = glm::perspectiveFov(glm::radians(45.f), static_cast<float>(width), static_cast<float>(height), 0.1f, 100.f);
      view = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
      model = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
      g_mvp = projection * view * model;
    }

    // Create uniform buffer
    _uniform_buffer = _buffer_factory.createBuffer(_renderer._device_object->_vk_device, sizeof(g_mvp), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    // Allocate uniform memory
    {
      auto memory_type_index = _renderer._physical_device_object->findProperties(_uniform_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      _uniform_memory = _renderer._device_memory_factory.createObject(_renderer._device_object, _uniform_buffer->_vk_memory_requirements.size, memory_type_index);
    }

    // Wrinte to memory
    {
      auto data = DeviceMemoryObject::vkMapMemory_(_renderer._device_object->_vk_device, _uniform_memory->_vk_device_memory, 0, _uniform_buffer->_vk_memory_requirements.size);

      memcpy(data, &g_mvp, sizeof(g_mvp));

      DeviceMemoryObject::vkUnmapMemory_(_renderer._device_object->_vk_device, _uniform_memory->_vk_device_memory);
    }

    // Bind memory to buffer
    BufferObject::vkBindBufferMemory_(_renderer._device_object->_vk_device, _uniform_buffer->_vk_buffer, _uniform_memory->_vk_device_memory, 0);

    // Create descriptor set layout
    _descriptor_set_layout = _descriptor_set_layout_factory.createObject(_renderer._device_object->_vk_device);
  }

  // Create Pipeline layout
  {
    VkPushConstantRange range = {};
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    range.offset = 0;
    range.size = sizeof(g_mvp);

    _pipeline_layout = _pipeline_layout_factory.createObject(_renderer._device_object->_vk_device, range, _descriptor_set_layout->_vk_descriptor_set_layout);
  }

  // Crearte descripter pool
  _descriptor_pool = _descriptor_pool_factory.createObject(_renderer._device_object->_vk_device);

  // Allocarte descriptor set
  _descriptor_set = _descriptor_pool->createObject(_renderer._device_object->_vk_device, _descriptor_set_layout->_vk_descriptor_set_layout);

  // Update descriptor sets

  VkDescriptorBufferInfo descriptor_buffer_info = {};
  descriptor_buffer_info.buffer = _uniform_buffer->_vk_buffer;
  descriptor_buffer_info.offset = 0;
  descriptor_buffer_info.range = sizeof(g_mvp);//VK_WHOLE_SIZE??

  DescriptorSetObject::vkUpdateDescriptorSets_(_renderer._device_object->_vk_device, _descriptor_set->_vk_descriptor_set, descriptor_buffer_info);

  // Create render pass
  {
    _render_pass_factory.getAttachmentDescriptionDepot().add("ColorDefaultDescription",
      _renderer._swapchain_object->_vk_format,
      VK_SAMPLE_COUNT_1_BIT,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_STORE,
      VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      VK_ATTACHMENT_STORE_OP_DONT_CARE,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );
    _render_pass_factory.getAttachmentDescriptionDepot().add("DepthDefaultDescription",
      _renderer._depth_image_object->_vk_format,
      VK_SAMPLE_COUNT_1_BIT,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_DONT_CARE,
      VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      VK_ATTACHMENT_STORE_OP_DONT_CARE,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );

    _render_pass_factory.getAttachmentReferenceDepot().add("ColorDefault",
      "ColorDefaultDescription",
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    _render_pass_factory.getAttachmentReferenceDepot().add("DepthDefault",
      "DepthDefaultDescription",
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    std::vector<const char*> color_attachment_names = { "ColorDefault" };
    _render_pass_factory.getSubpassDescriptionDepot().add("SubpassDefault",
      0,
      nullptr,
      static_cast<uint32_t>(color_attachment_names.size()),
      color_attachment_names.data(),
      nullptr,
      "DepthDefault",
      0,
      nullptr
    );

    _render_pass = _render_pass_factory.createObject(_renderer._device_object, { "ColorDefaultDescription", "DepthDefaultDescription" }, { "SubpassDefault" });
  }

  // Create Framebuffer
  {
    _framebuffers.reserve(_renderer._swapchain_object->_swapchain_image_count);
    for (uint32_t i = 0; i < _renderer._swapchain_object->_swapchain_image_count; i++) {
      _framebuffers.push_back(_framebuffer_factory.createObject(
        _renderer._device_object,
        _render_pass,
        { _renderer._swapchain_image_view_objects[i], _renderer._depth_image_view_object }));
    }
  }

  // Create shader module
  {
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
    auto ps_bin = get_binary(L"ps.frag.spv");

    _vs = _shader_module_factory.createObject(_renderer._device_object, vs_bin.second, reinterpret_cast<uint32_t*>(vs_bin.first.get()), VK_SHADER_STAGE_VERTEX_BIT, "main");
    _ps = _shader_module_factory.createObject(_renderer._device_object, ps_bin.second, reinterpret_cast<uint32_t*>(ps_bin.first.get()), VK_SHADER_STAGE_FRAGMENT_BIT, "main");
  }

  // Create Vertex buffer
  _vertex_buffer = _buffer_factory.createBuffer(_renderer._device_object->_vk_device, sizeof(poly), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

  // Allocate vertex buffer memory
  {
    auto memory_property_bits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    auto memory_type_index = _renderer._physical_device_object->findProperties(_vertex_buffer, memory_property_bits);
    _vertex_memory = _renderer._device_memory_factory.createObject(_renderer._device_object, _vertex_buffer->_vk_memory_requirements.size, memory_type_index);
  }

  // Store vertex buffer

  auto vertex_map = DeviceMemoryObject::vkMapMemory_(_renderer._device_object->_vk_device, _vertex_memory->_vk_device_memory, 0, _vertex_buffer->_vk_memory_requirements.size);

  memcpy(vertex_map, &poly, sizeof(poly));

  DeviceMemoryObject::vkUnmapMemory_(_renderer._device_object->_vk_device, _vertex_memory->_vk_device_memory);

  BufferObject::vkBindBufferMemory_(_renderer._device_object->_vk_device, _vertex_buffer->_vk_buffer, _vertex_memory->_vk_device_memory, 0);


  // Description

  VkVertexInputBindingDescription vertex_binding_description = {};
  vertex_binding_description.binding = 0;
  vertex_binding_description.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
  vertex_binding_description.stride = sizeof(Vertex);

  VkVertexInputAttributeDescription vertex_attribute_descriptions[] = {
    {
      0,
      0,
      VK_FORMAT_R32G32B32A32_SFLOAT,
      0
    },
    {
      1,
      0,
      VK_FORMAT_R32G32B32A32_SFLOAT,
      16
    }
  };

  VkPipelineVertexInputStateCreateInfo vertex_input_info = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &vertex_binding_description;
  vertex_input_info.vertexAttributeDescriptionCount = 2;
  vertex_input_info.pVertexAttributeDescriptions = vertex_attribute_descriptions;

  // Create pipeline
  _pipeline = _pipeline_factory.createObject(_renderer._device_object, nullptr, vertex_input_info, {_vs, _ps}, _pipeline_layout->_vk_pipeline_layout, _render_pass->_vk_render_pass);

  // Create semaphore
  _image_semaphore = _semaphore_factory.createSemaphore(_renderer._device_object->_vk_device);

  // Create fence
  _fence = _fence_factory.createFence(_renderer._device_object->_vk_device);
}
float a = 0;
void updateVulkan() {
  FenceObject::vkResetFence_(_renderer._device_object->_vk_device, _fence->_vk_fence);

  SwapchainObject::vkAcquireNextImage_(_renderer._device_object->_vk_device, _renderer._swapchain_object->_vk_swapchain, UINT64_MAX, _image_semaphore->_vk_semaphore, nullptr, &g_current_buffer);

  _renderer._command_buffer_object->begin();

  VkClearValue clear_values[2];
  clear_values[0].color = VkClearColorValue({{0.2f, 0.2f, 0.2f, 0.2f}});
  clear_values[1].depthStencil = VkClearDepthStencilValue({ 1.0f, 0 });

  VkRenderPassBeginInfo render_begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
  render_begin_info.renderPass = _render_pass->_vk_render_pass;
  render_begin_info.framebuffer = _framebuffers[g_current_buffer]->_vk_framebuffer;
  render_begin_info.renderArea = VkRect2D({ VkOffset2D({0, 0}), VkExtent2D({_width, _height}) });
  render_begin_info.clearValueCount = 2;
  render_begin_info.pClearValues = clear_values;

  _renderer._command_buffer_object->beginRenderPass(render_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  // ~~~

  _renderer._command_buffer_object->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->_vk_pipeline);

  uint32_t ofst = 0;
  _renderer._command_buffer_object->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout->_vk_pipeline_layout, 0, 1, &_descriptor_set->_vk_descriptor_set, 1, &ofst);

  VkViewport viewport = {};
  viewport.width = (float)_width;
  viewport.height = (float)_height;
  viewport.minDepth = 0;
  viewport.maxDepth = 1;
  VkRect2D scissor = {
    VkOffset2D { 0, 0 },
    VkExtent2D { _width, _height }
  };

  _renderer._command_buffer_object->setViewport(0, 1, &viewport);
  _renderer._command_buffer_object->setScissor(0, scissor);
  
  a+=0.01f;
  model = glm::translate(glm::mat4(1), glm::vec3(a, 0, 0));
  g_mvp = projection * view * model;
 
  vkCmdPushConstants(_renderer._command_buffer_object->_vk_command_buffer, _pipeline_layout->_vk_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(g_mvp), &g_mvp);

  vk::DeviceSize vertex_offset = 0;
  _renderer._command_buffer_object->bindVertexBuffers(0, _vertex_buffer->_vk_buffer, vertex_offset);


  uint32_t vertex_count = 2 * 3;
  uint32_t instance_count = 1;
  uint32_t first_vertex = 0;
  uint32_t first_instance = 0;

  _renderer._command_buffer_object->draw(vertex_count, instance_count, first_vertex, first_instance);

  // ~~~~

  _renderer._command_buffer_object->endRenderPass();

  _renderer._command_buffer_object->end();

  VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkCommandBuffer command_buffers[] = { _renderer._command_buffer_object->_vk_command_buffer };
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = command_buffers;

  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &_image_semaphore->_vk_semaphore;
  submit_info.pWaitDstStageMask = &pipe_stage_flags;
  vkQueueSubmit(_renderer._queue_object->_vk_queue, 1, &submit_info, _fence->_vk_fence);

  FenceObject::vkWaitForFence_(_renderer._device_object->_vk_device, _fence->_vk_fence, UINT64_MAX);

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &_renderer._swapchain_object->_vk_swapchain;
  present_info.pImageIndices = &g_current_buffer;

  vkQueuePresentKHR(_renderer._queue_object->_vk_queue, &present_info);
}

void uninitVulkan() {

  _fence_factory.destroyFence(_renderer._device_object->_vk_device, _fence);

  _pipeline_factory.destroyObject(_pipeline);

  _semaphore_factory.destroySemaphore(_renderer._device_object->_vk_device, _image_semaphore);

  _renderer._device_memory_factory.destroyObject(_vertex_memory);

  _buffer_factory.destroyBuffer(_renderer._device_object->_vk_device, _vertex_buffer);

  for (auto& framebuffer : _framebuffers) {
    _framebuffer_factory.destroyObject(framebuffer);
  }
  _framebuffers.clear();

  _shader_module_factory.destroyObject(_ps);

  _shader_module_factory.destroyObject(_vs);

  _render_pass_factory.destroyObject(_render_pass);

  _descriptor_pool->destroyObject(_renderer._device_object->_vk_device, _descriptor_set);

  _descriptor_pool_factory.destroyObject(_renderer._device_object->_vk_device, _descriptor_pool);

  _pipeline_layout_factory.destroyObject(_renderer._device_object->_vk_device, _pipeline_layout);

  _descriptor_set_layout_factory.destroyObject(_renderer._device_object->_vk_device, _descriptor_set_layout);

  _renderer._device_memory_factory.destroyObject(_uniform_memory);

  _buffer_factory.destroyBuffer(_renderer._device_object->_vk_device, _uniform_buffer);

  _renderer.uninit();
}

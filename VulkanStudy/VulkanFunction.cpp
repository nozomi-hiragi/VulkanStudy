
#include "VulkanFunction.h"
#include <vector>
#include <iostream>

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shaderc/shaderc.h>

#include "FenceObject.h"

#include "Transporter.h"

#include "Renderer.h"

#include "BufferFactory.h"
#include "ShaderModuleFactory.h"
#include "RenderPassFactory.h"
#include "FramebufferFactory.h"
#include "PipelineFactory.h"

#include "Mesh.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Renderer _renderer;

BufferFactory _buffer_factory;
ShaderModuleFactory _shader_module_factory;
RenderPassFactory _render_pass_factory;
FramebufferFactory _framebuffer_factory;
PipelineFactory _pipeline_factory;

std::shared_ptr<DeviceMemoryObject> _uniform_memory;
std::shared_ptr<BufferObject> _uniform_buffer;

std::shared_ptr<Mesh> _mesh;

std::shared_ptr<ShaderModuleObject> _vertex_shader;
std::shared_ptr<ShaderModuleObject> _pixel_shader;
std::shared_ptr<RenderPassObject> _render_pass;
std::vector<std::shared_ptr<FramebufferObject>> _framebuffers;
std::shared_ptr<PipelineObject> _pipeline;

uint32_t g_current_buffer = 0;

glm::mat4 g_mvp;

uint32_t _width;
uint32_t _height;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

void initVulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height) {
  _width = width;
  _height = height;

  _renderer.init(APP_NAME, APP_VERSION, width, height, hinstance, hwnd);

  _renderer._command_buffer_object->setViewSize(width, height);
  _renderer._command_buffer_object->setClearColorValue(0, VkClearColorValue({ {0.2f, 0.2f, 0.2f, 0.2f} }));
  _renderer._command_buffer_object->setClearDepthStencilValue(1, VkClearDepthStencilValue({ 1.0f, 0 }));

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
    std::string vs_code =
      "#version 450\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "#extension GL_ARB_shading_language_420pack : enable\n"
      "layout (binding = 0) uniform bufferVals {\n"
      "    mat4 mvp;\n"
      "} myBufferVals;\n"
      "layout (location = 0) in vec3 pos;\n"
      "layout (location = 1) in vec3 nor;\n"
      "layout (location = 2) in vec4 inColor;\n"
      "layout (location = 3) in vec2 tex;\n"
      "\n"
      "layout (location = 0) out vec4 outColor;\n"
      "void main() {\n"
      "   outColor = vec4(inColor.x,inColor.y,inColor.z, inColor.w);//inColor;\n"
      "   gl_Position = myBufferVals.mvp * vec4(pos.xyz, 1);\n"
      "}\n"
      ;

    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    shaderc_compilation_result_t result = shaderc_compile_into_spv(
      compiler, vs_code.c_str(), vs_code.size(),
      shaderc_glsl_vertex_shader, "vs_code", "main", nullptr);

    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
      std::cerr << shaderc_result_get_error_message(result) << std::endl;
    } 

    //auto vs_bin = GetBinary(L"vspc.vert.spv");
    auto ps_bin = GetBinary(L"ps.frag.spv");

    _vertex_shader = _shader_module_factory.createObject(_renderer._device_object, shaderc_result_get_length(result), reinterpret_cast<const uint32_t*>(shaderc_result_get_bytes(result)), VK_SHADER_STAGE_VERTEX_BIT, "main");
    //_vertex_shader = _shader_module_factory.createObject(_renderer._device_object, vs_bin.second, reinterpret_cast<uint32_t*>(vs_bin.first.get()), VK_SHADER_STAGE_VERTEX_BIT, "main");
    _pixel_shader = _shader_module_factory.createObject(_renderer._device_object, ps_bin.second, reinterpret_cast<uint32_t*>(ps_bin.first.get()), VK_SHADER_STAGE_FRAGMENT_BIT, "main");

    shaderc_result_release(result);
    shaderc_compiler_release(compiler);
  }

  // Define vertex input
  {
    _pipeline_factory.getVertexInputBindingDescriptionDepot().add("Vertex", 0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX);
    _pipeline_factory.getVertexInputBindingDescriptionDepot().add("Normal", 1, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX);
    _pipeline_factory.getVertexInputBindingDescriptionDepot().add("Color", 2, sizeof(glm::vec4), VK_VERTEX_INPUT_RATE_VERTEX);
    _pipeline_factory.getVertexInputBindingDescriptionDepot().add("Texcoord", 3, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX);

    _pipeline_factory.getVertexInputAttributeDescriptionDepot().add("Vertex", 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    _pipeline_factory.getVertexInputAttributeDescriptionDepot().add("Normal", 1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0);
    _pipeline_factory.getVertexInputAttributeDescriptionDepot().add("Color", 2, 2, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
    _pipeline_factory.getVertexInputAttributeDescriptionDepot().add("Texcoord", 3, 3, VK_FORMAT_R32G32_SFLOAT, 0);
  }

  // Create pipeline
  {
    std::vector<std::string> vertex_input_binding_description_names = { "Vertex", "Normal", "Color", "Texcoord" };
    std::vector<std::string> vertex_input_attribute_description_names = { "Vertex", "Normal", "Color", "Texcoord" };
    std::vector<std::shared_ptr<ShaderModuleObject>> shader_objects = { _vertex_shader, _pixel_shader };
    _pipeline = _pipeline_factory.createObject(_renderer._device_object,
      nullptr,
      vertex_input_binding_description_names,
      vertex_input_attribute_description_names,
      shader_objects,
      _renderer._pipeline_layout->_vk_pipeline_layout,
      _render_pass->_vk_render_pass);
  }

  // Create uniform buffer
  {
    // Create matrix
    {
      projection = glm::perspectiveFov(glm::radians(45.f), static_cast<float>(width), static_cast<float>(height), 0.1f, 100.f);
      view = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
      model = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
      g_mvp = projection * view * model;
    }

    // Create uniform buffer
    _uniform_buffer = _buffer_factory.createObject(_renderer._device_object, sizeof(g_mvp), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

    // Allocate uniform memory
    {
      auto memory_type_index = _renderer._physical_device_object->findProperties(_uniform_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      _uniform_memory = _renderer._device_memory_factory.createObject(_renderer._device_object, _uniform_buffer->_vk_memory_requirements.size, memory_type_index);
    }

    // Wrinte to memory
    {
      auto data = _uniform_memory->mapMemory(_renderer._device_object, 0, _uniform_buffer->_vk_memory_requirements.size);
      memcpy(data, &g_mvp, sizeof(g_mvp));
      _uniform_memory->unmapMemory(_renderer._device_object);
    }

    // Bind memory to buffer
    _uniform_buffer->bindBufferMemory(_renderer._device_object, _uniform_memory, 0);
  }

  // Update descriptor sets

  VkDescriptorBufferInfo descriptor_buffer_info = {};
  descriptor_buffer_info.buffer = _uniform_buffer->_vk_buffer;
  descriptor_buffer_info.offset = 0;
  descriptor_buffer_info.range = sizeof(g_mvp);//VK_WHOLE_SIZE??

  DescriptorSetObject::vkUpdateDescriptorSets_(_renderer._device_object->_vk_device, _renderer._descriptor_set->_vk_descriptor_set, descriptor_buffer_info);

  // Create mesh
  {
    std::vector<glm::vec3> pos = {
      glm::vec3(-1, +1, 0),
      glm::vec3(+1, +1, 0),
      glm::vec3(-1, -1, 0),
    };

    std::vector<glm::vec3> nor = {
      glm::vec3(0, 0, 1),
      glm::vec3(0, 0, 1),
      glm::vec3(0, 0, 1),
    };

    std::vector<glm::vec4> col = {
      glm::vec4(1, 0, 0, 1),
      glm::vec4(0, 1, 0, 1),
      glm::vec4(0, 0, 1, 1),
    };

    std::vector<glm::vec2> tex = {
      glm::vec2(0, 0),
      glm::vec2(1, 0),
      glm::vec2(0, 1),
    };

    _mesh = std::make_shared<Mesh>(pos, nor, col, tex, _buffer_factory, _renderer._device_memory_factory, _renderer._physical_device_object, _renderer._device_object);
  }
}
float a = 0;
void updateVulkan() {
  a += 0.01f;
  model = glm::translate(glm::mat4(1), glm::vec3(a, 0, 0));
  g_mvp = projection * view * model;

  {
    auto data = _uniform_memory->mapMemory(_renderer._device_object, 0, _uniform_buffer->_vk_memory_requirements.size);
    memcpy(data, &g_mvp, sizeof(g_mvp));
    _uniform_memory->unmapMemory(_renderer._device_object);
  }

  _renderer._fence->resetFence(_renderer._device_object);

  _renderer._swapchain_object->acquireNextImage(_renderer._device_object, UINT64_MAX, _renderer._semaphore, nullptr, &g_current_buffer);

  _renderer._command_buffer_object->begin();

  _renderer._command_buffer_object->beginRenderPass(_render_pass, _framebuffers[g_current_buffer], VK_SUBPASS_CONTENTS_INLINE);

  _renderer._command_buffer_object->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

  uint32_t ofst = 0;
  _renderer._command_buffer_object->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, _renderer._pipeline_layout->_vk_pipeline_layout, 0, 1, &_renderer._descriptor_set->_vk_descriptor_set, 1, &ofst);

  _mesh->draw(_renderer._command_buffer_object);

  _renderer._command_buffer_object->endRenderPass();

  _renderer._command_buffer_object->end();

  VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkCommandBuffer command_buffers[] = { _renderer._command_buffer_object->_vk_command_buffer };
  VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &_renderer._semaphore->_vk_semaphore;
  submit_info.pWaitDstStageMask = &pipe_stage_flags;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = command_buffers;
  submit_info.signalSemaphoreCount = 0;
  submit_info.pSignalSemaphores = nullptr;

  _renderer._queue_object->submit(1, &submit_info, _renderer._fence);

  _renderer._fence->waitForFence(_renderer._device_object, UINT64_MAX);

  VkPresentInfoKHR present_info = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  present_info.waitSemaphoreCount = 0;
  present_info.pWaitSemaphores = nullptr;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &_renderer._swapchain_object->_vk_swapchain;
  present_info.pImageIndices = &g_current_buffer;

  _renderer._queue_object->present(present_info);
}

void uninitVulkan() {
  _mesh.reset();

  _pipeline_factory.destroyObject(_pipeline);

  for (auto& framebuffer : _framebuffers) {
    _framebuffer_factory.destroyObject(framebuffer);
  }
  _framebuffers.clear();

  _shader_module_factory.destroyObject(_pixel_shader);

  _shader_module_factory.destroyObject(_vertex_shader);

  _render_pass_factory.destroyObject(_render_pass);

  _renderer._device_memory_factory.destroyObject(_uniform_memory);

  _buffer_factory.destroyObject(_uniform_buffer);

  _renderer.uninit();
}

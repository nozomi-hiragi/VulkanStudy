#pragma once

#include <iostream>
#include <Windows.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <shaderc/shaderc.h>

#include "InstanceFactory.h"
#include "SurfaceFactory.h"
#include "DeviceFactory.h"
#include "FenceObject.h"

#include "Transporter.h"

#include "BufferFactory.h"
#include "ShaderModuleFactory.h"
#include "RenderPassFactory.h"
#include "FramebufferFactory.h"
#include "PipelineFactory.h"
#include "SamplerFactory.h"
#include "QueueDepot.h"
#include "PipelineLayoutFactory.h"
#include "CommandPoolFactory.h"
#include "SwapchainFactory.h"
#include "ImageViewFactory.h"
#include "SemaphoreFactory.h"
#include "FenceFactory.h"
#include "ImageFactory.h"
#include "DescriptorSetLayoutFactory.h"
#include "DescriptorPoolFactory.h"
#include "DeviceMemoryFactory.h"
#include "ConstantBufferLayoutFactory.h"

#include "Mesh.h"
#include "DynamicUniformBufferRing.h"
#include "VertexLayoutBuilder.h"

class MeshStatus {
public:
  MeshStatus() : _is_exist(false), _instance(nullptr), _position(0, 0, 0), _rotation(0, 0, 0), _scale(1, 1, 1) {
  }

  MeshStatus(std::shared_ptr<Mesh> mesh) : _is_exist(true), _instance(mesh), _position(0, 0, 0), _rotation(0, 0, 0), _scale(1, 1, 1) {
  }

  ~MeshStatus() {
  }

  auto getInstance() const {
    return _instance;
  }

  void setInstance(std::shared_ptr<Mesh> instance) {
    _is_exist = instance != nullptr;
    _instance = instance;
  }

  const bool isExist() const {
    return _is_exist;
  }

  void clear() {
    _is_exist = false;
    _instance.reset();
  }

  auto createMatrix(const glm::mat4& vp) {
    return glm::scale(
      glm::translate(
        vp,
        _position
      ) * glm::yawPitchRoll(
        _rotation.y, _rotation.x, _rotation.z
      ),
      _scale
    );
  }

  glm::vec3 _position;
  glm::vec3 _rotation;
  glm::vec3 _scale;

protected:
private:
  bool _is_exist;
  std::shared_ptr<Mesh> _instance;
};

class Renderer {
public:
  Renderer()
    :_render_pass_factory(_attachment_description_depot, _attachment_reference_depot, _subpass_description_depot)
  {
  }

  ~Renderer() {
  }

  void init(const char* app_name, const uint32_t app_version, const uint32_t width, const uint32_t height, GLFWwindow* window) {
    _width = width;
    _height = height;

    Order<InstanceObject, InstanceParams> order;
    order.params.app_name = app_name;
    order.params.app_version = app_version;
    order.address = [this, &window](Borrowed<InstanceObject> borrowed) {
      _instance = borrowed;

      _physical_device_object = borrowed.getObject()->_physical_devices[0];
      _surface_object = _surface_factory.createObject(borrowed.getObject(), window);
      _device_object = _device_factory.createObject(nullptr, _physical_device_object, _surface_object);
    };

    _instance_factory.borrowinRgequest(order);


    _queue_object = _device_object->_queue_object;
    _command_pool_object = _command_pool_factory.createObject(_device_object, _queue_object);
    _command_buffer_object = _command_pool_object->createObject(_device_object);
    _swapchain_object = _swapchain_factory.createObject(_device_object, _physical_device_object, _surface_object, _width, _height);
    _depth_image_object = _image_factory.createObject(_device_object, VK_FORMAT_D16_UNORM, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, _width, _height, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);
    {
      auto memory_type_index = _physical_device_object->findProperties(_depth_image_object, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      _depth_memory_object = _device_memory_factory.createObject(_device_object, _depth_image_object->_vk_memory_requirements.size, memory_type_index);
    }
    _depth_image_object->bindImageMemory(_device_object, _depth_memory_object, 0);
    _swapchain_image_view_objects.reserve(_swapchain_object->_swapchain_image_count);
    for (auto image : _swapchain_object->_swapchain_images) {
      _swapchain_image_view_objects.push_back(_image_view_factory.createObject(_device_object, image));
    }
    _depth_image_view_object = _image_view_factory.createObject(_device_object, _depth_image_object);
    _semaphore = _semaphore_factory.createObject(_device_object);
    _fence = _fence_factory.createObject(_device_object);

    _command_buffer_object->setViewSize(_width, _height);
    _command_buffer_object->setClearColorValue(0, VkClearColorValue({ {0.2f, 0.2f, 0.2f, 0.2f} }));
    _command_buffer_object->setClearDepthStencilValue(1, VkClearDepthStencilValue({ 1.0f, 0 }));

    std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings = {
      {
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        1,
        VK_SHADER_STAGE_VERTEX_BIT,
        nullptr
      },
      {
        1,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        1,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        nullptr
      }
    };
    _constant_buffer_layout = _constant_buffer_layout_factory.createObject(_device_object, descriptor_set_layout_bindings);

    // Create render pass
    {
      _attachment_description_depot.add("ColorDefaultDescription",
        _swapchain_object->_vk_format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      );
      _attachment_description_depot.add("DepthDefaultDescription",
        _depth_image_object->_vk_format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
      );

      _attachment_reference_depot.add("ColorDefault",
        "ColorDefaultDescription",
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      _attachment_reference_depot.add("DepthDefault",
        "DepthDefaultDescription",
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

      std::vector<const char*> color_attachment_names = { "ColorDefault" };
      _subpass_description_depot.add("SubpassDefault",
        0,
        nullptr,
        static_cast<uint32_t>(color_attachment_names.size()),
        color_attachment_names.data(),
        nullptr,
        "DepthDefault",
        0,
        nullptr
      );

      _render_pass = _render_pass_factory.createObject(_device_object, { "ColorDefaultDescription", "DepthDefaultDescription" }, { "SubpassDefault" });
    }

    // Create uniform buffer
    {
      uint32_t uniform_size = 16 * 1024 * 1024;
      _uniform_buffer = std::make_shared<DynamicUniformBufferRing>(_device_object, _physical_device_object, _buffer_factory, _device_memory_factory, uniform_size);
    }

    // Texture
    std::vector<uint8_t> raw_texture = {
      0xff, 0x00, 0x00, 0xff,  0x00, 0xff, 0x00, 0xff,  0x00, 0x00, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff,  0x00, 0x00, 0xff, 0xff,  0x00, 0xff, 0x00, 0xff,  0xff, 0x00, 0x00, 0xff,
      0xff, 0x00, 0x00, 0xff,  0x00, 0xff, 0x00, 0xff,  0x00, 0x00, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
    };

    _texture_image = _image_factory.createObject(_device_object,
      VK_FORMAT_R8G8B8A8_UNORM,
      VK_IMAGE_USAGE_SAMPLED_BIT,
      4,
      3,
      1,
      VK_SAMPLE_COUNT_1_BIT,
      VK_IMAGE_TILING_LINEAR,
      VK_IMAGE_ASPECT_COLOR_BIT);

    auto memory_property_bits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    auto memory_type_index = _physical_device_object->findProperties(_texture_image, memory_property_bits);
    _texture_memory = _device_memory_factory.createObject(_device_object, _texture_image->_vk_memory_requirements.size, memory_type_index);
    _texture_image->bindImageMemory(_device_object, _texture_memory, 0);

    VkImageSubresource image_subresource = {};
    image_subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_subresource.mipLevel = 0;
    image_subresource.arrayLayer = 0;
    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(_device_object->_vk_device, _texture_image->_vk_image, &image_subresource, &layout);

    char* data = (char*)_texture_memory->mapMemory(_device_object, 0, static_cast<uint32_t>(raw_texture.size() * sizeof(uint8_t)));
    auto row_size = sizeof(uint8_t) * 4 * 4;
    for (uint32_t i = 0; i < 3; i++) {
      memcpy(data + i * layout.rowPitch, raw_texture.data() + row_size * i, row_size);
    }
    _texture_memory->unmapMemory(_device_object);

    _texture_image_view = _image_view_factory.createObject(_device_object, _texture_image);

    _sampler_object = _sampler_factory.createObject(_device_object, VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST, 0, 0.f);

    // Update descriptor sets
    _constant_buffer_layout->_descriptor_set->updateDescriptorSetBuffer(_device_object, 0, _uniform_buffer->_buffer, sizeof(glm::mat4));
    _constant_buffer_layout->_descriptor_set->updateDescriptorSetSampler(_device_object, 1, _sampler_object, _texture_image_view);

    // Texture convert?
    {
      auto command_buffer = _command_pool_object->createObject(_device_object);
      command_buffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

      command_buffer->pipelineImageMemoryBarrier(
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        _texture_image,
        0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

      command_buffer->end();

      _queue_object->_submit_depot.add("Texture",
        {}, nullptr, { command_buffer }, {});
      _queue_object->registSubmitInfoName(1, { "Texture" });
      _queue_object->submit(1, _fence);
      _fence->waitForFence(_device_object);
      _command_pool_object->destroyObject(command_buffer);
    }

    //
    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    _queue_object->_submit_depot.add("Submit",
      { _semaphore },
      &pipe_stage_flags,
      { _command_buffer_object },
      {});

    _queue_object->registSubmitInfoName(0, { "Submit" });

    _shader_compiler = shaderc_compiler_initialize();
  }

  auto createShaderModule(std::string code, std::string name, VkShaderStageFlagBits stage) {
    shaderc_shader_kind kind;
    switch (stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:
      kind = shaderc_vertex_shader;
      break;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
      kind = shaderc_tess_control_shader;
      break;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
      kind = shaderc_tess_evaluation_shader;
      break;
    case VK_SHADER_STAGE_GEOMETRY_BIT:
      kind = shaderc_geometry_shader;
      break;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
      kind = shaderc_fragment_shader;
      break;
    case VK_SHADER_STAGE_COMPUTE_BIT:
      kind = shaderc_compute_shader;
      break;
    default:
      return std::make_shared<ShaderModuleObject>(nullptr, stage, nullptr);
    }

    shaderc_compilation_result_t result = shaderc_compile_into_spv(
      _shader_compiler, code.c_str(), code.size(),
      kind, name.c_str(), "main", nullptr);
    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
      std::cerr << shaderc_result_get_error_message(result) << std::endl;
    }
    auto shader_module = _shader_module_factory.createObject(_device_object, shaderc_result_get_length(result), reinterpret_cast<const uint32_t*>(shaderc_result_get_bytes(result)), stage, "main");

    //auto bin = GetBinary(L".spv");
    //shader = _shader_module_factory.createObject(_device_object, ps_bin.second, reinterpret_cast<uint32_t*>(bin.first.get()), stage, "main");

    shaderc_result_release(result);
    return shader_module;
  }

  void destroyShaderModule(std::shared_ptr<ShaderModuleObject>& shader) {
    _shader_module_factory.destroyObject(shader);
  }

  void createPipeline(std::shared_ptr<ShaderModuleObject> vs, std::shared_ptr<ShaderModuleObject> ps) {
    auto vertex_layout = Mesh::createVertexLayout();
    _pipeline = _pipeline_factory.createObject(_device_object,
      nullptr,
      vertex_layout,
      { vs, ps },
      _constant_buffer_layout->_pipeline_layout,
      _render_pass);
  }

  void createSwapchainFrameBuffer() {
    _framebuffers.reserve(_swapchain_object->_swapchain_image_count);
    for (uint32_t i = 0; i < _swapchain_object->_swapchain_image_count; i++) {
      _framebuffers.push_back(_framebuffer_factory.createObject(
        _device_object,
        _render_pass,
        { _swapchain_image_view_objects[i], _depth_image_view_object }
      ));
    }
  }

  auto createMesh(const std::vector<glm::vec3>& position, const std::vector<glm::vec3>& normal, const std::vector<glm::vec4>& color, const std::vector<glm::vec2>& texcoord, const std::vector<uint16_t>& index) {
    auto mesh = std::make_shared<Mesh>(position, normal, color, texcoord, index, _buffer_factory, _device_memory_factory, _physical_device_object, _device_object);
    _mesh_status.push_back(std::make_shared<MeshStatus>(mesh));
    return _mesh_status.back();
  }

  void uninit() {
    shaderc_compiler_release(_shader_compiler);

    _fence->waitForFence(_device_object);

    _sampler_factory.destroyObject(_sampler_object);

    _image_view_factory.destroyObject(_texture_image_view);

    _device_memory_factory.destroyObject(_texture_memory);

    _image_factory.destroyObject(_texture_image);

    for (auto& it : _mesh_status) {
      it->clear();
    }
    _mesh_status.clear();

    _pipeline_factory.destroyObject(_pipeline);

    for (auto& framebuffer : _framebuffers) {
      _framebuffer_factory.destroyObject(framebuffer);
    }
    _framebuffers.clear();

    _shader_module_factory.destroyAll();

    _render_pass_factory.destroyObject(_render_pass);

    _uniform_buffer.reset();

    _buffer_factory.executeDestroy();

    _constant_buffer_layout_factory.destroyObject(_constant_buffer_layout);
    _constant_buffer_layout_factory.destroyAll();

    _fence_factory.destroyObject(_fence);
    _semaphore_factory.destroyObject(_semaphore);
    _image_view_factory.destroyObject(_depth_image_view_object);
    for (auto& image_view : _swapchain_image_view_objects) {
      _image_view_factory.destroyObject(image_view);
    }
    _swapchain_image_view_objects.clear();
    _device_memory_factory.destroyObject(_depth_memory_object);
    _image_factory.destroyObject(_depth_image_object);
    _swapchain_factory.destroyObject(_swapchain_object);
    _command_pool_object->destroyObject(_command_buffer_object);
    _command_pool_factory.destroyObject(_command_pool_object);

    _device_factory.destroyObject(_device_object);
    _surface_factory.destroyObject(_surface_object);
    _instance.returnObject();
  }

  void beginCommand() {
    _fence->waitForFence(_device_object);

    _fence->resetFence(_device_object);

    _swapchain_object->acquireNextImage(_device_object, UINT64_MAX, _semaphore, nullptr, &g_current_buffer);

    _buffer_factory.executeDestroy();

    _command_buffer_object->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

    _command_buffer_object->applyViewSize();

    _command_buffer_object->beginRenderPass(_render_pass, _framebuffers[g_current_buffer], VK_SUBPASS_CONTENTS_INLINE);

    _command_buffer_object->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
  }

  void endCommand() {
    _command_buffer_object->endRenderPass();

    _command_buffer_object->end();

    _queue_object->submit(0, _fence);

    VkPresentInfoKHR present_info = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 0;
    present_info.pWaitSemaphores = nullptr;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &_swapchain_object->_vk_swapchain;
    present_info.pImageIndices = &g_current_buffer;

    _queue_object->present(present_info);
  }

  void update(glm::mat4 vp) {
    for (const auto& it : _mesh_status) {
      if (!it->isExist()) { continue; }
      auto mvp = it->createMatrix(vp);

      uint32_t offset;
      auto data = _uniform_buffer->getPointer(sizeof(mvp), &offset);
      memcpy(data, &mvp, sizeof(mvp));

      _command_buffer_object->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, _constant_buffer_layout, 1, &offset);
      it->getInstance()->draw(_command_buffer_object);
    }
  }

protected:
private:
  shaderc_compiler_t _shader_compiler;

  AttachmentDescriptionDepot _attachment_description_depot;
  AttachmentReferenceDepot _attachment_reference_depot;
  SubpassDescriptionDepot _subpass_description_depot;

  InstanceFactory _instance_factory;
  SurfaceFactory _surface_factory;
  DeviceFactory _device_factory;

  BufferFactory _buffer_factory;
  ShaderModuleFactory _shader_module_factory;
  RenderPassFactory _render_pass_factory;
  FramebufferFactory _framebuffer_factory;
  PipelineFactory _pipeline_factory;
  SamplerFactory _sampler_factory;

  CommandPoolFactory _command_pool_factory;
  SwapchainFactory _swapchain_factory;
  ImageFactory _image_factory;
  DeviceMemoryFactory _device_memory_factory;
  ImageViewFactory _image_view_factory;
  SemaphoreFactory _semaphore_factory;
  FenceFactory _fence_factory;
  ConstantBufferLayoutFactory _constant_buffer_layout_factory;

  Borrowed<InstanceObject> _instance;
  std::shared_ptr<PhysicalDeviceObject> _physical_device_object;
  std::shared_ptr<SurfaceObject> _surface_object;
  std::shared_ptr<DeviceObject> _device_object;

  std::vector<std::shared_ptr<MeshStatus>> _mesh_status;
  std::shared_ptr<RenderPassObject> _render_pass;
  std::vector<std::shared_ptr<FramebufferObject>> _framebuffers;
  std::shared_ptr<PipelineObject> _pipeline;
  std::shared_ptr<ImageObject> _texture_image;
  std::shared_ptr<DeviceMemoryObject> _texture_memory;
  std::shared_ptr<ImageViewObject> _texture_image_view;
  std::shared_ptr<SamplerObject> _sampler_object;

  std::shared_ptr<QueueObject> _queue_object;
  std::shared_ptr<CommandPoolObject> _command_pool_object;
  std::shared_ptr<CommandBufferObject> _command_buffer_object;
  std::shared_ptr<SwapchainObject> _swapchain_object;
  std::shared_ptr<ImageObject> _depth_image_object;
  std::shared_ptr<DeviceMemoryObject> _depth_memory_object;
  std::vector<std::shared_ptr<ImageViewObject>> _swapchain_image_view_objects;
  std::shared_ptr<ImageViewObject> _depth_image_view_object;
  std::shared_ptr<SemaphoreObject> _semaphore;
  std::shared_ptr<FenceObject> _fence;
  std::shared_ptr<ConstantBufferLayoutObject> _constant_buffer_layout;

  std::shared_ptr<DynamicUniformBufferRing> _uniform_buffer;

  uint32_t _width;
  uint32_t _height;

  uint32_t g_current_buffer = 0;
};

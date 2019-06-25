
#include "VulkanFunction.h"
#include <vector>
#include <fstream>

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Surface.h"
#include "Device.h"
#include "Queue.h"
#include "CommandPool.h"
#include "CommandBuffer.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Instance myInstance;
PhysicalDevice myPhysicalDevice;
vk::PhysicalDeviceMemoryProperties g_physical_device_memoryproperties; // ÉÅÉÇÉää÷åWÇ…Ç‹Ç∆ÇﬂÇÁÇÍÇªÇ§
Surface mySurface;
uint32_t g_graphics_queue_family_index = UINT32_MAX;
uint32_t g_present_queue_family_index = UINT32_MAX;
Device myDevice;
CommandPool myCommandPool;
CommandBuffer myCommandBuffer;
Queue myQueue;
vk::SwapchainKHR g_swapchain = nullptr;
vk::Format g_surface_format;
std::vector<vk::Image> g_swapchain_images;
std::vector<vk::ImageView> g_swapchain_image_views;
vk::Format g_depth_format;
vk::Image g_depth_image = nullptr;
vk::DeviceMemory g_depth_memory = nullptr;
vk::ImageView g_depth_image_view = nullptr;
vk::Buffer g_uniform_buffer = nullptr;
vk::DeviceMemory g_uniform_memory = nullptr;
vk::MemoryRequirements g_uniform_buffer_memory_requirements;
vk::DescriptorSetLayout g_descriptor_set_layout = nullptr;
vk::PipelineLayout g_pipeline_layout = nullptr;
vk::DescriptorPool g_descriptor_pool = nullptr;
vk::DescriptorSet g_descriptor_set = nullptr;
vk::RenderPass g_render_pass = nullptr;
vk::ShaderModule g_vs = nullptr;
vk::ShaderModule g_ps = nullptr;
std::vector<vk::Framebuffer> g_frame_buffers;
vk::Buffer g_vertex_buffer = nullptr;
vk::DeviceMemory g_vertex_memory = nullptr;
vk::Semaphore g_image_semaphore = nullptr;
vk::Pipeline g_pipeline = nullptr;
vk::Fence g_fence = nullptr;

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

void initVulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height) {

  // Create Instance
  myInstance.createInstance(APP_NAME, APP_VERSION);

  // Get Physical Device
  {
    myPhysicalDevice = myInstance.getPhysicalDevice(0);
    g_physical_device_memoryproperties = myPhysicalDevice.getMemoryProperties();
  }

  // Create Surface
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  mySurface = myInstance.createSurface(hinstance, hwnd);
#endif

  // Create Device
  {
    auto queue_family_properties = myPhysicalDevice.getQueueFamilyProperties();

    g_graphics_queue_family_index = UINT32_MAX;
    g_present_queue_family_index = UINT32_MAX;
    for (uint32_t i = 0; i < queue_family_properties.size(); i++) {
      if (g_graphics_queue_family_index == UINT32_MAX
        && queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
        g_graphics_queue_family_index = i;
      }
      if (g_present_queue_family_index == UINT32_MAX && myPhysicalDevice.getSurfaceSupport(i, mySurface)) {
        g_present_queue_family_index = i;
      }
      if (g_graphics_queue_family_index != UINT32_MAX && g_present_queue_family_index != UINT32_MAX) {
        break;
      }
    }

    float queue_prioritie = 0.f;
    vk::DeviceQueueCreateInfo queue;
    queue.setQueueCount(1);
    queue.setQueueFamilyIndex(g_graphics_queue_family_index);
    queue.setPQueuePriorities(&queue_prioritie);

    myDevice = myPhysicalDevice.createDevice(queue);
  }

  // Create Command pool
  myCommandPool = myDevice.createCommandPool(g_graphics_queue_family_index);

  // Create primaly Command buffer
  myCommandBuffer = myDevice.allocateCommandBuffer(myCommandPool);

  //

  myQueue = myDevice.getQueue(g_graphics_queue_family_index, 0);

  // Create Swapchain
  {
    auto surface_capabilities = myPhysicalDevice.getSurfaceCapabilities(mySurface);

    auto surface_formats = myPhysicalDevice.getSurfaceFormats(mySurface);
    g_surface_format = surface_formats[0].format;
    if (surface_formats.size() == 1 && g_surface_format == vk::Format::eUndefined) {
      g_surface_format = vk::Format::eB8G8R8A8Unorm;
    }
    auto surface_color_space = surface_formats[0].colorSpace;

    vk::SurfaceTransformFlagBitsKHR pre_transform;
    if (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
      pre_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    }
    else {
      pre_transform = surface_capabilities.currentTransform;
    }

    vk::CompositeAlphaFlagBitsKHR composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    vk::CompositeAlphaFlagBitsKHR composite_alpha_flags[] = {
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
        vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
        vk::CompositeAlphaFlagBitsKHR::eInherit,
    };
    for (auto it : composite_alpha_flags) {
      if (surface_capabilities.supportedCompositeAlpha & it) {
        composite_alpha = it;
        break;
      }
    }

    auto swapchain_info = vk::SwapchainCreateInfoKHR()
      .setSurface(mySurface.getVkSurface())
      .setMinImageCount(surface_capabilities.minImageCount)
      .setImageFormat(g_surface_format)
      .setImageColorSpace(surface_color_space)
      .setImageExtent({ width, height })
      .setImageArrayLayers(1)
      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
      .setImageSharingMode(vk::SharingMode::eExclusive)
      .setQueueFamilyIndexCount(0)
      .setPQueueFamilyIndices(nullptr)
      .setPreTransform(pre_transform)
      .setCompositeAlpha(composite_alpha)
      .setPresentMode(vk::PresentModeKHR::eFifo)
      .setClipped(true)
      .setOldSwapchain(nullptr);

    g_swapchain = myDevice.createSwapchain(swapchain_info);

    g_swapchain_images = myDevice.getSwapchainImages(g_swapchain);
  }

  // Create Swapchain image view
  {
    g_swapchain_image_views.clear();
    g_swapchain_image_views.reserve(g_swapchain_images.size());

    for (auto image : g_swapchain_images) {
      auto color_image_view = vk::ImageViewCreateInfo()
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(g_surface_format)
        .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
        .setImage(image);

      g_swapchain_image_views.push_back(myDevice.createImageView(color_image_view));
    }
  }

  // Create Depth image
  {
    g_depth_format = vk::Format::eD16Unorm;
    auto const depth_image_info = vk::ImageCreateInfo()
      .setImageType(vk::ImageType::e2D)
      .setFormat(g_depth_format)
      .setExtent({ width, height, 1 })
      .setMipLevels(1)
      .setArrayLayers(1)
      .setSamples(vk::SampleCountFlagBits::e1)
      .setTiling(vk::ImageTiling::eOptimal)
      .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
      .setSharingMode(vk::SharingMode::eExclusive)
      .setQueueFamilyIndexCount(0)
      .setPQueueFamilyIndices(nullptr)
      .setInitialLayout(vk::ImageLayout::eUndefined);

    g_depth_image = myDevice.createImage(depth_image_info);
  }

  // Allocate depth memory
  {
    auto depth_image_memory_requirements = myDevice.getImageMemoryRequirements(g_depth_image);

    auto memory_type_bits = depth_image_memory_requirements.memoryTypeBits;
    auto memory_property_bits = vk::MemoryPropertyFlagBits::eDeviceLocal;
    auto memory_types = g_physical_device_memoryproperties.memoryTypes;
    auto memory_type_index = 0;
    for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
      if (memory_type_bits & 1) {
        if ((memory_types[i].propertyFlags & memory_property_bits) == memory_property_bits) {
          memory_type_index = i;
          break;
        }
      }
      memory_type_bits >>= 1;
    }

    auto depth_memory_info = vk::MemoryAllocateInfo()
      .setAllocationSize(depth_image_memory_requirements.size)
      .setMemoryTypeIndex(memory_type_index);

    g_depth_memory = myDevice.allocateMemory(depth_memory_info);
  }

  // Bind memory to depth image
  {
    myDevice.bindImageMemory(g_depth_image, g_depth_memory, 0);
  }

  // Create depth image view
  {
    auto depth_image_view_info = vk::ImageViewCreateInfo()
      .setImage(g_depth_image)
      .setViewType(vk::ImageViewType::e2D)
      .setFormat(g_depth_format)
      .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));

    g_depth_image_view = myDevice.createImageView(depth_image_view_info);
  }

  // Create matrix
  {
    auto projection = glm::perspectiveFov(glm::radians(45.f), static_cast<float>(width), static_cast<float>(height), 0.1f, 100.f);
    auto view = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
    auto model = glm::mat4(1);
    g_mvp = projection * view * model;
  }

  // Create uniform buffer
  {
    auto uniform_buffer_info = vk::BufferCreateInfo()
      .setSize(sizeof(g_mvp))
      .setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

    g_uniform_buffer = myDevice.createBuffer(uniform_buffer_info);
  }

  // Allocate uniform memory
  {
    g_uniform_buffer_memory_requirements = myDevice.getBufferMemoryRequirements(g_uniform_buffer);

    auto memory_type_bits = g_uniform_buffer_memory_requirements.memoryTypeBits;
    auto memory_property_bits = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto memory_types = g_physical_device_memoryproperties.memoryTypes;
    auto memory_type_index = 0;
    for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
      if (memory_type_bits & 1) {
        if ((memory_types[i].propertyFlags & memory_property_bits) == memory_property_bits) {
          memory_type_index = i;
          break;
        }
      }
      memory_type_bits >>= 1;
    }

    auto uniform_memory_info = vk::MemoryAllocateInfo()
      .setAllocationSize(g_uniform_buffer_memory_requirements.size)
      .setMemoryTypeIndex(memory_type_index);

    g_uniform_memory = myDevice.allocateMemory(uniform_memory_info);
  }

  // Wrinte to memory
  {
    auto data = myDevice.mapMemory(g_uniform_memory, 0, g_uniform_buffer_memory_requirements.size);

    memcpy(data, &g_mvp, sizeof(g_mvp));

    myDevice.unmapMemory(g_uniform_memory);
  }

  // Bind memory to buffer
  {
    myDevice.bindBufferMemory(g_uniform_buffer, g_uniform_memory, 0);
  }

  // Create descriptor set layout
  {
    auto descriptor_set_layout_binding = vk::DescriptorSetLayoutBinding()
      .setBinding(0)
      .setDescriptorType(vk::DescriptorType::eUniformBuffer)
      .setDescriptorCount(1)
      .setStageFlags(vk::ShaderStageFlagBits::eVertex)
      .setPImmutableSamplers(nullptr);

    auto descriptor_set_layout_info = vk::DescriptorSetLayoutCreateInfo()
      .setBindingCount(1)
      .setPBindings(&descriptor_set_layout_binding);

    g_descriptor_set_layout = myDevice.createDescriptorSetLayout(descriptor_set_layout_info);
  }

  // Create Pipeline layout
  {
    auto pipeline_layout_info = vk::PipelineLayoutCreateInfo()
      .setSetLayoutCount(1)
      .setPSetLayouts(&g_descriptor_set_layout);

    g_pipeline_layout = myDevice.createPipelineLayout(pipeline_layout_info);
  }

  // Crearte descripter pool

  vk::DescriptorPoolSize pool_sizes[] = {
    vk::DescriptorPoolSize()
    .setType(vk::DescriptorType::eUniformBuffer)
    .setDescriptorCount(1),
  };

  auto descriptor_pool_info = vk::DescriptorPoolCreateInfo()
    .setMaxSets(1)
    .setPoolSizeCount(1)
    .setPPoolSizes(pool_sizes);

  g_descriptor_pool = myDevice.createDescriptorPool(descriptor_pool_info);

  // Allocarte descriptor set

  auto descriptor_set_info = vk::DescriptorSetAllocateInfo()
    .setDescriptorPool(g_descriptor_pool)
    .setDescriptorSetCount(1)
    .setPSetLayouts(&g_descriptor_set_layout);

  g_descriptor_set = myDevice.allocateDescriptorSets(descriptor_set_info)[0];

  // Update descriptor sets

  auto descriptor_buffer_info = vk::DescriptorBufferInfo()
    .setOffset(0)
    .setRange(sizeof(g_mvp))
    .setBuffer(g_uniform_buffer);

  vk::WriteDescriptorSet write_descriptor_sets[] = {
    vk::WriteDescriptorSet()
    .setDescriptorCount(1)
    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
    .setPBufferInfo(&descriptor_buffer_info)
    .setDstSet(g_descriptor_set)
  };

  myDevice.updateDescriptorSets(1, write_descriptor_sets, 0, nullptr);

  // Create render pass

  vk::AttachmentDescription attachment_description[] = {
    vk::AttachmentDescription()
    .setFormat(g_surface_format)
    .setSamples(vk::SampleCountFlagBits::e1)
    .setLoadOp(vk::AttachmentLoadOp::eClear)
    .setStoreOp(vk::AttachmentStoreOp::eStore)
    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
    .setInitialLayout(vk::ImageLayout::eUndefined)
    .setFinalLayout(vk::ImageLayout::ePresentSrcKHR),

    vk::AttachmentDescription()
    .setFormat(g_depth_format)
    .setSamples(vk::SampleCountFlagBits::e1)
    .setLoadOp(vk::AttachmentLoadOp::eClear)
    .setStoreOp(vk::AttachmentStoreOp::eDontCare)
    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
    .setInitialLayout(vk::ImageLayout::eUndefined)
    .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
  };

  auto color_reference = vk::AttachmentReference()
    .setAttachment(0)
    .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

  auto depth_reference = vk::AttachmentReference()
    .setAttachment(1)
    .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto subpass_description = vk::SubpassDescription()
    .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
    .setInputAttachmentCount(0)
    .setPInputAttachments(nullptr)
    .setColorAttachmentCount(1)
    .setPColorAttachments(&color_reference)
    .setPResolveAttachments(nullptr)
    .setPDepthStencilAttachment(&depth_reference)
    .setPreserveAttachmentCount(0)
    .setPPreserveAttachments(nullptr);

  auto render_pass_info = vk::RenderPassCreateInfo()
    .setAttachmentCount(2)
    .setPAttachments(attachment_description)
    .setSubpassCount(1)
    .setPSubpasses(&subpass_description)
    .setDependencyCount(0)
    .setPDependencies(nullptr);

  g_render_pass = myDevice.createRenderPass(render_pass_info);

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

  auto vs_bin = get_binary(L"vs.vert.spv");

  auto vs_info = vk::ShaderModuleCreateInfo()
    .setCodeSize(vs_bin.second)
    .setPCode(reinterpret_cast<uint32_t*>(vs_bin.first.get()));

  g_vs = myDevice.createShaderModule(vs_info);

  auto ps_bin = get_binary(L"ps.frag.spv");

  auto ps_info = vk::ShaderModuleCreateInfo()
    .setCodeSize(ps_bin.second)
    .setPCode(reinterpret_cast<uint32_t*>(ps_bin.first.get()));

  g_ps = myDevice.createShaderModule(ps_info);

  vk::PipelineShaderStageCreateInfo shader_stage_info[] = {
    vk::PipelineShaderStageCreateInfo()
    .setStage(vk::ShaderStageFlagBits::eVertex)
    .setModule(g_vs)
    .setPName("main"),

    vk::PipelineShaderStageCreateInfo()
    .setStage(vk::ShaderStageFlagBits::eFragment)
    .setModule(g_ps)
    .setPName("main")
  };

  //

  vk::ImageView attachments[2];
  attachments[1] = g_depth_image_view;

  auto const frame_buffer_info = vk::FramebufferCreateInfo()
    .setRenderPass(g_render_pass)
    .setAttachmentCount(2)
    .setPAttachments(attachments)
    .setWidth(width)
    .setHeight(height)
    .setLayers(1);

  g_frame_buffers.reserve(g_swapchain_images.size());

  for (uint32_t i = 0; i < g_swapchain_images.size(); i++) {
    attachments[0] = g_swapchain_image_views[i];
    g_frame_buffers.push_back(myDevice.createFramebuffer(frame_buffer_info));
  }

  // Create Vertex buffer

  auto vertex_buffer_info = vk::BufferCreateInfo()
    .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
    .setSize(sizeof(poly))
    .setSharingMode(vk::SharingMode::eExclusive);

  g_vertex_buffer = myDevice.createBuffer(vertex_buffer_info);

  // Allocate vertex buffer memory

  auto vertex_buffer_memory_requirements = myDevice.getBufferMemoryRequirements(g_vertex_buffer);

  auto memory_type_bits = vertex_buffer_memory_requirements.memoryTypeBits;
  auto memory_property_bits = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
  auto memory_types = g_physical_device_memoryproperties.memoryTypes;
  auto memory_type_index = 0;
  for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
    if (memory_type_bits & 1) {
      if ((memory_types[i].propertyFlags & memory_property_bits) == memory_property_bits) {
        memory_type_index = i;
        break;
      }
    }
    memory_type_bits >>= 1;
  }

  auto vertex_memory_info = vk::MemoryAllocateInfo()
    .setAllocationSize(vertex_buffer_memory_requirements.size)
    .setMemoryTypeIndex(memory_type_index);

  g_vertex_memory = myDevice.allocateMemory(vertex_memory_info);

  // Store vertex buffer

  auto vertex_map = myDevice.mapMemory(g_vertex_memory, 0, vertex_buffer_memory_requirements.size);

  memcpy(vertex_map, &poly, sizeof(poly));

  myDevice.unmapMemory(g_vertex_memory);

  myDevice.bindBufferMemory(g_vertex_buffer, g_vertex_memory, 0);

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

  g_image_semaphore = myDevice.createSemaphore(vk::SemaphoreCreateInfo());

  myDevice.acquireNextImage(g_swapchain, UINT64_MAX, g_image_semaphore, nullptr, &g_current_buffer);

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

  auto viewport_info = vk::PipelineViewportStateCreateInfo()
    .setViewportCount(1)
    .setScissorCount(1);

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
    .setPDynamicState(&dynamic_state_info)
    .setLayout(g_pipeline_layout)
    .setRenderPass(g_render_pass);

  g_pipeline = myDevice.createGraphicsPipeline(nullptr, pipeline_info);

  // Begin command

  myCommandBuffer.begin();

  // Begin render pass

  vk::ClearValue clear_values[2] = {
    vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})),
    vk::ClearDepthStencilValue(1.0f, 0u)
  };

  auto render_begin_info = vk::RenderPassBeginInfo()
    .setRenderPass(g_render_pass)
    .setFramebuffer(g_frame_buffers[g_current_buffer])
    .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(width, height)))
    .setClearValueCount(2)
    .setPClearValues(clear_values);

  myCommandBuffer.beginRenderPass(render_begin_info, vk::SubpassContents::eInline);

  //

  myCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, g_pipeline);

  //

  myCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, g_pipeline_layout, 0, 1, &g_descriptor_set, 0, nullptr);

  //

  auto viewport = vk::Viewport()
    .setWidth((float)width)
    .setHeight((float)height)
    .setMinDepth((float)0.0f)
    .setMaxDepth((float)1.0f);

  myCommandBuffer.setViewport(0, 1, &viewport);

  // Bind vertex buffer

  vk::DeviceSize vertex_offset = 0;
  myCommandBuffer.bindVertexBuffers(0, g_vertex_buffer, vertex_offset);

  //

  vk::Rect2D scissor(vk::Offset2D(0, 0), vk::Extent2D(width, height));
  myCommandBuffer.setScissor(0, scissor);

  //

  uint32_t vertex_count = 2 * 3;
  uint32_t instance_count = 1;
  uint32_t first_vertex = 0;
  uint32_t first_instance = 0;

  myCommandBuffer.draw(vertex_count, instance_count, first_vertex, first_instance);

  // End render pass

  myCommandBuffer.endRenderPass();

  // End command

  myCommandBuffer.end();

  //

  auto fence_info = vk::FenceCreateInfo();

  g_fence = myDevice.createFence(fence_info);

  //

  vk::PipelineStageFlags pipe_stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  vk::CommandBuffer command_buffers[] = { myCommandBuffer.getVkCommandBuffer() };
  auto submit_info = vk::SubmitInfo()
    .setCommandBufferCount(1)
    .setPCommandBuffers(command_buffers)

    .setWaitSemaphoreCount(1)
    .setPWaitSemaphores(&g_image_semaphore)
    .setPWaitDstStageMask(&pipe_stage_flags);

  myQueue.submit(submit_info, g_fence);

  //

  vk::Result res;

  do {
    res = myDevice.waitForFences(1, &g_fence, true, UINT64_MAX);
  } while (res == vk::Result::eTimeout);

  //

  auto present_info = vk::PresentInfoKHR()
    .setSwapchainCount(1)
    .setPSwapchains(&g_swapchain)
    .setPImageIndices(&g_current_buffer);

  myQueue.present(present_info);

}

void updateVulkan() {
}

void uninitVulkan() {

  if (g_fence) {
    myDevice.destroyFence(g_fence);
    g_fence = nullptr;
  }

  if (g_pipeline) {
    myDevice.destroyPipeline(g_pipeline);
    g_pipeline = nullptr;
  }

  if (g_image_semaphore) {
    myDevice.destroySemaphore(g_image_semaphore);
    g_image_semaphore = nullptr;
  }

  if (g_vertex_memory) {
    myDevice.freeMemory(g_vertex_memory);
    g_vertex_memory = nullptr;
  }

  if (g_vertex_buffer) {
    myDevice.destroyBuffer(g_vertex_buffer);
    g_vertex_buffer = nullptr;
  }

  for (auto frame_buffer : g_frame_buffers) {
    myDevice.destroyFramebuffer(frame_buffer);
  }
  g_frame_buffers.clear();

  if (g_ps) {
    myDevice.destroyShaderModule(g_ps);
    g_ps = nullptr;
  }

  if (g_vs) {
    myDevice.destroyShaderModule(g_vs);
    g_vs = nullptr;
  }

  if (g_render_pass) {
    myDevice.destroyRenderPass(g_render_pass);
    g_render_pass = nullptr;
  }

  if (g_descriptor_pool) {
    myDevice.destroyDescriptorPool(g_descriptor_pool);
    g_descriptor_pool = nullptr;
  }

  if (g_pipeline_layout) {
    myDevice.destroyPipelineLayout(g_pipeline_layout);
    g_pipeline_layout = nullptr;
  }

  if (g_descriptor_set_layout) {
    myDevice.destroyDescriptorSetLayout(g_descriptor_set_layout);
    g_descriptor_set_layout = nullptr;
  }

  if (g_uniform_memory) {
    myDevice.freeMemory(g_uniform_memory);
    g_uniform_memory = nullptr;
  }

  if (g_uniform_buffer) {
    myDevice.destroyBuffer(g_uniform_buffer);
    g_uniform_buffer = nullptr;
  }

  if (g_depth_image_view) {
    myDevice.destroyImageView(g_depth_image_view);
    g_depth_image_view = nullptr;
  }

  if (g_depth_memory) {
    myDevice.freeMemory(g_depth_memory);
    g_depth_memory = nullptr;
  }

  if (g_depth_image) {
    myDevice.destroyImage(g_depth_image);
    g_depth_image = nullptr;
  }

  myDevice.freeCommandBuffers(myCommandPool, myCommandBuffer);

  myDevice.destroyCommandPool(myCommandPool);

  for (auto image_view : g_swapchain_image_views) {
    myDevice.destroyImageView(image_view);
  }
  g_swapchain_image_views.clear();

  g_swapchain_images.clear();

  if (g_swapchain) {
    myDevice.destroySwapchain(g_swapchain);
    g_swapchain = nullptr;
  }

  myDevice.destroy();

  myInstance.destroySurface(mySurface);

  myInstance.destroyInstance();
}

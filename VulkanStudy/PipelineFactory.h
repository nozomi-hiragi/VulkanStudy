#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

#include "PipelineObject.h"

class PipelineFactory {
  static VkPipeline _createVkGraphicsPipeline(VkDevice device, const VkPipelineCache pipeline_cache, uint32_t width, uint32_t height, VkPipelineVertexInputStateCreateInfo& vertex_input_info, VkPipelineShaderStageCreateInfo* shader_stages, VkPipelineLayout pipeline_layout, VkRenderPass render_pass) {
    //VkDynamicState dynamic_states[2] = {
    //  VK_DYNAMIC_STATE_VIEWPORT,
    //  VK_DYNAMIC_STATE_SCISSOR
    //};

    //VkPipelineDynamicStateCreateInfo dynamic_state_info = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    //dynamic_state_info.pDynamicStates = dynamic_states;
    //dynamic_state_info.dynamicStateCount = 2;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rasterization_info = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterization_info.depthClampEnable = VK_FALSE;
    rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization_info.depthBiasEnable = VK_FALSE;
    rasterization_info.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState color_blend_attachments[1] = {
      {
        VK_FALSE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
      }
    };

    VkPipelineColorBlendStateCreateInfo color_blend_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = color_blend_attachments;

    VkViewport viewport = {};
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;

    VkRect2D scissor = {
      VkOffset2D { 0, 0 },
      VkExtent2D { width, height }
    };

    VkPipelineViewportStateCreateInfo viewport_info = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &viewport;
    viewport_info.scissorCount = 1;
    viewport_info.pScissors = &scissor;

    VkStencilOpState stencil_op = {};
    stencil_op.failOp = VK_STENCIL_OP_KEEP;
    stencil_op.passOp = VK_STENCIL_OP_KEEP;
    stencil_op.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    depth_stencil_info.depthTestEnable = VK_TRUE;
    depth_stencil_info.depthWriteEnable = VK_TRUE;
    depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_info.stencilTestEnable = VK_FALSE;
    depth_stencil_info.front = stencil_op;
    depth_stencil_info.back = stencil_op;

    VkPipelineMultisampleStateCreateInfo multisample_info = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkGraphicsPipelineCreateInfo pipeline_info = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &rasterization_info;
    pipeline_info.pMultisampleState = &multisample_info;
    pipeline_info.pDepthStencilState = &depth_stencil_info;
    pipeline_info.pColorBlendState = &color_blend_info;
    pipeline_info.pDynamicState = nullptr/*&dynamic_state_info*/;
    pipeline_info.layout = pipeline_layout;
    pipeline_info.renderPass = render_pass;

    VkPipeline pipeline;
    vkCreateGraphicsPipelines(device, pipeline_cache, 1, &pipeline_info, nullptr, &pipeline);
    return pipeline;
  }

  static void _destroyVkPipeline(VkDevice device, const VkPipeline pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
  }

  std::shared_ptr<PipelineObject> _createCore(VkDevice device, const VkPipelineCache pipeline_cache, uint32_t width, uint32_t height, VkPipelineVertexInputStateCreateInfo& vertex_input_info, VkPipelineShaderStageCreateInfo* shader_stages, VkPipelineLayout pipeline_layout, VkRenderPass render_pass) {
    auto vk_descriptor_pool = _createVkGraphicsPipeline(device, pipeline_cache, width, height, vertex_input_info, shader_stages, pipeline_layout, render_pass);
    return std::make_shared<PipelineObject>(vk_descriptor_pool);
  }

  void _destroyCore(VkDevice device, std::shared_ptr<PipelineObject> object) {
    _destroyVkPipeline(device, object->_vk_pipeline);
  }

public:
  std::shared_ptr<PipelineObject> createObject(VkDevice device, const VkPipelineCache pipeline_cache, uint32_t width, uint32_t height, VkPipelineVertexInputStateCreateInfo& vertex_input_info, VkPipelineShaderStageCreateInfo* shader_stages, VkPipelineLayout pipeline_layout, VkRenderPass render_pass) {
    auto object = _createCore(device, pipeline_cache, width, height, vertex_input_info, shader_stages, pipeline_layout, render_pass);
    _container.insert(object);
    return object;
  }

  void destroyObject(VkDevice device, std::shared_ptr<PipelineObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyCore(device, object);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<PipelineObject>> _container;
};

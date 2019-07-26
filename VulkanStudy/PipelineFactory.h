#pragma once

#include <vulkan/vulkan.h>

#include "AbstractFactory.h"
#include "PipelineObject.h"
#include "DeviceObject.h"
#include "ShaderModuleObject.h"

class PipelineFactory : public AbstractFactory<PipelineObject, DeviceObject, const VkPipelineCache, VkPipelineVertexInputStateCreateInfo&, const std::vector<std::shared_ptr<ShaderModuleObject>>&, const VkPipelineLayout, const VkRenderPass> {
public:
protected:
private:
  static VkPipeline _createVkGraphicsPipeline(VkDevice device, const VkPipelineCache pipeline_cache, VkPipelineVertexInputStateCreateInfo& vertex_input_info, VkPipelineShaderStageCreateInfo* shader_stages, VkPipelineLayout pipeline_layout, VkRenderPass render_pass) {
    VkDynamicState dynamic_states[2] = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamic_state_info.pDynamicStates = dynamic_states;
    dynamic_state_info.dynamicStateCount = 2;

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

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.blendEnable         = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_MULTIPLY_EXT;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_MULTIPLY_EXT;
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachments[] = {
      color_blend_attachment
    };

    VkPipelineColorBlendStateCreateInfo color_blend_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    color_blend_info.attachmentCount = sizeof(color_blend_attachments) / sizeof(VkPipelineColorBlendAttachmentState);
    color_blend_info.pAttachments = color_blend_attachments;

    VkPipelineViewportStateCreateInfo viewport_info = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewport_info.viewportCount = 1;
    viewport_info.scissorCount = 1;

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
    pipeline_info.pDynamicState = &dynamic_state_info;
    pipeline_info.layout = pipeline_layout;
    pipeline_info.renderPass = render_pass;

    VkPipeline pipeline;
    vkCreateGraphicsPipelines(device, pipeline_cache, 1, &pipeline_info, nullptr, &pipeline);
    return pipeline;
  }

  static void _destroyVkPipeline(VkDevice device, const VkPipeline pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
  }

  std::shared_ptr<PipelineObject> _createCore(const VkPipelineCache pipeline_cache, VkPipelineVertexInputStateCreateInfo& vertex_input_info, const std::vector<std::shared_ptr<ShaderModuleObject>>& shader_modules, const VkPipelineLayout pipeline_layout, const VkRenderPass render_pass) {
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
    shader_stages.reserve(shader_modules.size());
    for (const auto& it : shader_modules) {
      VkPipelineShaderStageCreateInfo shader_stage_info = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
      shader_stage_info.stage = it->_vk_shader_stage;
      shader_stage_info.module = it->_vk_shader_module;
      shader_stage_info.pName = it->_entry;
      shader_stages.push_back(shader_stage_info);
    }
    auto vk_descriptor_pool = _createVkGraphicsPipeline(_parent->_vk_device, pipeline_cache, vertex_input_info, shader_stages.data(), pipeline_layout, render_pass);
    return std::make_shared<PipelineObject>(vk_descriptor_pool);
  }

  void _destroyCore(std::shared_ptr<PipelineObject> object) {
    _destroyVkPipeline(_parent->_vk_device, object->_vk_pipeline);
  }

};

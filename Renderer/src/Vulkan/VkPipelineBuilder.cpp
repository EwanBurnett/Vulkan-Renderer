#include "../../include/Vulkan/VkPipelineBuilder.h"
#include "../../include/Vulkan/VkContext.h"
#include "../../include/Logger.h"

VKR::VkPipelineBuilder::VkPipelineBuilder() {
    
}

VkResult VKR::VkPipelineBuilder::BuildGraphicsPipeline(const VkContext& context, const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpassIdx, VkPipeline* pPipeline) const
{
    const VkGraphicsPipelineCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, 
        nullptr, 
        0, 
        m_ShaderStages.size(), 
        m_ShaderStages.data(), 
        &m_VertexInputState, 
        &m_InputAssemblyState, 
        &m_TessellationState, 
        &m_ViewportState,
        &m_RasterizerState, 
        &m_MSAAState, 
        &m_DepthStencilState, 
        &m_BlendState, 
        &m_DynamicState, 
        layout, 
        renderPass, 
        subpassIdx, 
        nullptr, //TODO: Pipeline Derivatives
        -1
    };

    return vkCreateGraphicsPipelines(context.GetDevice(), nullptr, 1, &createInfo, nullptr, pPipeline); 
}

VkResult VKR::VkPipelineBuilder::BuildComputePipeline(const VkContext& context , const VkPipelineLayout layout, VkPipeline* pPipeline) const
{
    if (m_ShaderStages.size() < 1 || m_ShaderStages.size() > 1) {
        Log::Warning("[Vulkan]\tBuildComputePipeline() was called, but Shader Stage count is invalid. Compute shaders must have exactly 1 shader bound.\n");
    }

    if (!m_ShaderStages.at(0).stage == VK_SHADER_STAGE_COMPUTE_BIT) {
        Log::Warning("[Vulkan]\tCompute Pipeline Shader Stage was not VK_SHADER_STAGE_COMPUTE_BIT!\n");
    }

    const VkComputePipelineCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        nullptr,
        0,
        m_ShaderStages.at(0),
        layout,
        nullptr,    //TODO: Pipeline Derivatives
        -1,
    };

    return vkCreateComputePipelines(context.GetDevice(), nullptr, 1, &createInfo, nullptr, pPipeline); //TODO: Pipeline Cache

}

void VKR::VkPipelineBuilder::AddShaderStage(const VkShaderModule shaderModule, const VkShaderStageFlagBits stage, const char* entryPoint)
{
    const VkPipelineShaderStageCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr,
        0,
        stage,
        shaderModule,
        entryPoint
    };

    m_ShaderStages.push_back(createInfo);
}

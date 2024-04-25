#include "../../include/Vulkan/VkPipelineBuilder.h"
#include "../../include/Vulkan/VkContext.h"
#include "../../include/Vulkan/VkInit.h"
#include "../../include/Logger.h"
#include <easy/profiler.h>

VKR::VkPipelineBuilder::VkPipelineBuilder() {
    //Default Initialize our Pipeline State
    m_ShaderStages.clear();
    m_VertexInputState = VkInit::MakePipelineVertexInputStateCreateInfo();
    m_InputAssemblyState = VkInit::MakePipelineInputAssemblyStateCreateInfo();
    m_TessellationState = VkInit::MakePipelineTessellationStateCreateInfo();
    m_ViewportState = VkInit::MakePipelineViewportStateCreateInfo();
    m_RasterizerState = VkInit::MakePipelineRasterizationStateCreateInfo();
    m_MSAAState = VkInit::MakePipelineMultisampleStateCreateInfo();
    m_DepthStencilState = VkInit::MakePipelineDepthStencilStateCreateInfo();
    m_BlendState = VkInit::MakePipelineColorBlendStateCreateInfo();
    m_DynamicState = VkInit::MakePipelineDynamicStateCreateInfo();
}

const VkGraphicsPipelineCreateInfo VKR::VkPipelineBuilder::BuildGraphicsPipeline(const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpassIdx) const
{
    EASY_FUNCTION(profiler::colors::Red300);

    //Lambda to validate Pipeline before creation. 
    auto isValidPipeline = [this]() {
        if (m_ShaderStages.empty()) {
            Log::Warning("[Vulkan]\tGraphics Pipeline Shader Stages array is Empty!\n");
            return false;
        }

        //All graphics pipelines must at least have a Vertex and Fragment shader bound. 
        bool vertexShaderFound = false;
        bool fragmentShaderFound = false;

        for (const VkPipelineShaderStageCreateInfo& stage : m_ShaderStages) {
            if (stage.stage == VK_SHADER_STAGE_COMPUTE_BIT)
            {
                Log::Warning("[Vulkan]\tGraphics Pipeline Shader Stage %d was VK_PIPELINE_STAGE_COMPUTE_BIT!\n");
                return false;
            }
            //Only allow one Vertex shader
            if (stage.stage == VK_SHADER_STAGE_VERTEX_BIT && !vertexShaderFound) {
                vertexShaderFound = true;
            }
            if (stage.stage == VK_SHADER_STAGE_FRAGMENT_BIT && !fragmentShaderFound) {
                fragmentShaderFound = true;
            }
        }

        if (!vertexShaderFound) {
            Log::Warning("[Vulkan]\tGraphics Pipeline Missing Vertex Shader Stage!\n");
            return false;
        }
        if (!fragmentShaderFound) {
            Log::Warning("[Vulkan]\tGraphics Pipeline Missing Fragment Shader Stage!\n");
            return false;
        }

        return true;
    };

    if (!isValidPipeline()) {
        Log::Warning("[Vulkan]\tInvalid Pipeline!\n");
    }


    return {
          VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
          nullptr,
          0,
          static_cast<uint32_t>(m_ShaderStages.size()),
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

    //return vkCreateGraphicsPipelines(context.GetDevice(), nullptr, 1, &createInfo, nullptr, pPipeline);
}

const VkComputePipelineCreateInfo VKR::VkPipelineBuilder::BuildComputePipeline(const VkPipelineLayout layout) const
{
    EASY_FUNCTION(profiler::colors::Red300);
    if (m_ShaderStages.size() < 1 || m_ShaderStages.size() > 1) {
        Log::Warning("[Vulkan]\tBuildComputePipeline() was called, but Shader Stage count is invalid. Compute shaders must have exactly 1 shader bound.\n");
    }

    if (!m_ShaderStages.at(0).stage == VK_SHADER_STAGE_COMPUTE_BIT) {
        Log::Warning("[Vulkan]\tCompute Pipeline Shader Stage was not VK_SHADER_STAGE_COMPUTE_BIT!\n");
    }

    return {
        VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        nullptr,
        0,
        m_ShaderStages.at(0),
        layout,
        nullptr,    //TODO: Pipeline Derivatives
        -1,
    };

    //return vkCreateComputePipelines(context.GetDevice(), nullptr, 1, &createInfo, nullptr, pPipeline); //TODO: Pipeline Cache
}

void VKR::VkPipelineBuilder::AddShaderStage(const VkShaderModule shaderModule, const VkShaderStageFlagBits stage, const char* entryPoint)
{
    EASY_FUNCTION(profiler::colors::Red300);
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

void VKR::VkPipelineBuilder::SetVertexInputState(const uint32_t numBindings, const VkVertexInputBindingDescription* pBindings, const uint32_t numAttributes, const VkVertexInputAttributeDescription* pAttributes)
{
    m_VertexInputState.vertexBindingDescriptionCount = numBindings;
    m_VertexInputState.pVertexBindingDescriptions = pBindings;
    m_VertexInputState.vertexAttributeDescriptionCount = numAttributes;
    m_VertexInputState.pVertexAttributeDescriptions = pAttributes;
}

void VKR::VkPipelineBuilder::SetInputAssemblyState(const VkPrimitiveTopology primitiveTopology, const bool primitiveRestartEnable)
{
    m_InputAssemblyState.primitiveRestartEnable = primitiveRestartEnable;
    m_InputAssemblyState.topology = primitiveTopology;
}

void VKR::VkPipelineBuilder::SetTessellationState(const uint32_t numPatchControlPoints)
{
    m_TessellationState.patchControlPoints = numPatchControlPoints;
}

void VKR::VkPipelineBuilder::SetViewportState(const uint32_t numViewports, const VkViewport* pViewports, const uint32_t numScissors, const VkRect2D* pScissors)
{
    m_ViewportState.viewportCount = numViewports;
    m_ViewportState.pViewports = pViewports;
    m_ViewportState.scissorCount = numScissors;
    m_ViewportState.pScissors = pScissors;
}

void VKR::VkPipelineBuilder::SetRasterizerState(const VkPolygonMode fillMode, const VkCullModeFlags cullMode, const VkFrontFace frontFace, const bool depthClampEnable, const bool discardEnable)
{
    m_RasterizerState.polygonMode = fillMode;
    m_RasterizerState.cullMode = cullMode;
    m_RasterizerState.frontFace = frontFace;
    m_RasterizerState.depthClampEnable = depthClampEnable;
    m_RasterizerState.rasterizerDiscardEnable = discardEnable;
}

void VKR::VkPipelineBuilder::SetMSAAState(const VkSampleCountFlagBits sampleCount, const bool alphaToCoverageEnable, const bool alphaToOneEnable, const bool sampleShadingEnable, const float minSampleShading, const VkSampleMask* pSampleMask)
{
    m_MSAAState.rasterizationSamples = sampleCount;
    m_MSAAState.sampleShadingEnable = sampleShadingEnable;
    m_MSAAState.minSampleShading = minSampleShading;
    m_MSAAState.pSampleMask = pSampleMask;
    m_MSAAState.alphaToCoverageEnable = alphaToCoverageEnable;
    m_MSAAState.alphaToOneEnable = alphaToOneEnable;
}

void VKR::VkPipelineBuilder::SetDepthStencilState(const bool depthEnable, const bool depthWriteEnable, const bool stencilEnable, const VkCompareOp compareOp)
{
    m_DepthStencilState.depthTestEnable = depthEnable;
    m_DepthStencilState.depthWriteEnable = depthWriteEnable;
    m_DepthStencilState.stencilTestEnable = stencilEnable;
    m_DepthStencilState.depthCompareOp = compareOp;
}

void VKR::VkPipelineBuilder::SetBlendState(const bool logicOpEnable, const VkLogicOp logicOp, const uint32_t numAttachments, const VkPipelineColorBlendAttachmentState* pAttachments)
{
    m_BlendState.logicOpEnable = logicOpEnable;
    m_BlendState.logicOp = logicOp;
    m_BlendState.attachmentCount = numAttachments;
    m_BlendState.pAttachments = pAttachments;
}

void VKR::VkPipelineBuilder::SetDynamicState(const uint32_t numDynamicStates, const VkDynamicState* pDynamicStates)
{
    m_DynamicState.dynamicStateCount = numDynamicStates; 
    m_DynamicState.pDynamicStates = pDynamicStates; 
}


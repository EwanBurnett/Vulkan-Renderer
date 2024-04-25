#ifndef __VKRENDERER_VKPIPELINEBUILDER_H
#define __VKRENDERER_VKPIPELINEBUILDER_H
/**
*   @file VkPipelineBuilder.h
*   @brief Vulkan Pipeline Creation Utility
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/25
*/

#include "VkCommon.h"
#include <vector> 

namespace VKR {
    class VkContext;

    class VkPipelineBuilder {
    public:
        VkPipelineBuilder();

        const VkGraphicsPipelineCreateInfo BuildGraphicsPipeline(const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpassIdx) const;
        const VkComputePipelineCreateInfo BuildComputePipeline(const VkPipelineLayout layout) const;

        void AddShaderStage(const VkShaderModule shaderModule, const VkShaderStageFlagBits stage, const char* entryPoint);

        void SetVertexInputState(const uint32_t numBindings, const VkVertexInputBindingDescription* pBindings, const uint32_t numAttributes, const VkVertexInputAttributeDescription* pAttributes);

        void SetInputAssemblyState(const VkPrimitiveTopology primitiveTopology, const bool primitiveRestartEnable);

        void SetTessellationState(const uint32_t numPatchControlPoints);

        void SetViewportState(const uint32_t numViewports, const VkViewport* pViewports, const uint32_t numScissors, const VkRect2D* pScissors);

        void SetRasterizerState(const VkPolygonMode fillMode, const VkCullModeFlags cullMode, const VkFrontFace frontFace, const bool depthClampEnable = false, const bool discardEnable = false);

        void SetMSAAState(const VkSampleCountFlagBits sampleCount, const bool alphaToCoverageEnable, const bool alphaToOneEnable, const bool sampleShadingEnable = false, const float minSampleShading = 1.0f, const VkSampleMask* pSampleMask = nullptr);

        /**
         * @brief Sets the Depth-Stencil State of the Pipeline.
         * @param depthEnable Controls whether Depth testing is enabled. 
         * @param depthWriteEnable Controls whether Depth Writes are enabled. 
         * @param stencilEnable Controls whether Stencil testing is enabled. 
         * @param compareOp A Comparison operator used within the Depth Test. 
         * @remark This function does not currently support custom Depth-Bounds Tests.
         * @sa https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#fragops-dbt
        */
        void SetDepthStencilState(const bool depthEnable, const bool depthWriteEnable, const bool stencilEnable, const VkCompareOp compareOp = VK_COMPARE_OP_LESS);  

        void SetBlendState(const bool logicOpEnable, const VkLogicOp logicOp, const uint32_t numAttachments, const VkPipelineColorBlendAttachmentState* pAttachments);

        void SetDynamicState(const uint32_t numDynamicStates, const VkDynamicState* pDynamicStates);



    private:
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
        VkPipelineVertexInputStateCreateInfo m_VertexInputState;
        VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyState;
        VkPipelineTessellationStateCreateInfo m_TessellationState; 
        VkPipelineViewportStateCreateInfo m_ViewportState;
        VkPipelineRasterizationStateCreateInfo m_RasterizerState;
        VkPipelineMultisampleStateCreateInfo m_MSAAState;
        VkPipelineDepthStencilStateCreateInfo m_DepthStencilState;
        VkPipelineColorBlendStateCreateInfo m_BlendState;
        VkPipelineDynamicStateCreateInfo m_DynamicState;
    };
}

#endif
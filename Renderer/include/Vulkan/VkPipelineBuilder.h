#ifndef __VKRENDERER_VKPIPELINEBUILDER_H
#define __VKRENDERER_VKPIPELINEBUILDER_H
#include "VkCommon.h"
#include <vector> 

namespace VKR {
    class VkContext; 

    class VkPipelineBuilder {
    public: 
        VkPipelineBuilder(); 

        VkResult BuildGraphicsPipeline(const VkContext& context, const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpassIdx, VkPipeline* pPipeline) const;
        VkResult BuildComputePipeline(const VkContext& context , const VkPipelineLayout layout, VkPipeline* pPipeline) const; 

        void AddShaderStage(const VkShaderModule shaderModule, const VkShaderStageFlagBits stage, const char* entryPoint); 

        void SetVertexInputState(const uint32_t numBindings, const VkVertexInputBindingDescription* pBindings, const uint32_t numAttributes, const VkVertexInputAttributeDescription* pAttributes);

        void SetInputAssemblyState(const VkPrimitiveTopology primitiveTopology, const bool primitiveRestartEnable);

        void SetViewportState(const uint32_t numViewports, const VkViewport* pViewports, const uint32_t numScissors, const VkRect2D* pScissors);

        void SetRasterizerState(const VkPolygonMode fillMode, const VkCullModeFlags cullMode, const VkFrontFace frontFace, const bool depthClampEnable = false, const bool discardEnable = false);

        void SetMSAAState(const VkSampleCountFlagBits sampleCount, const bool alphaToCoverageEnable, const bool alphaToOneEnable, const bool sampleShadingEnable = false, const float minSampleShading = 1.0f, const VkSampleMask* pSampleMask = nullptr);

        void SetDepthStencilState(); //TODO: Depth Stencil State Configuration 

        void SetBlendState(const bool logicOpEnable, const VkLogicOp logicOp, const uint32_t numAttachments, const VkPipelineColorBlendAttachmentState* pAttachments); 

        void SetDynamicState(); //TODO: Dynamic State Configuration


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
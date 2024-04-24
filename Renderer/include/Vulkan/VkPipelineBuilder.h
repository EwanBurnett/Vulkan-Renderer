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
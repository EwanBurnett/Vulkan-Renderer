#ifndef __HELPERS_H
#define __HELPERS_H

#include <VKR/Vulkan/VkCommon.h>

/**
* @brief Wrapper for the resources required for images.
*/
struct ImageResource {
    VkImage image;
    VmaAllocation allocation;
    VkImageView view;
};

/**
 * @brief Wrapper for buffer resources.
*/
struct BufferResource {
    VkBuffer buffer;
    VmaAllocation allocation;
};

/**
 * @brief Storage class for Pipeline Statistics
*/
struct PipelineStatistics
{
    uint64_t inputAssemblyVertices;
    uint64_t inputAssemblyPrimitives;
    uint64_t vertexShaderInvocations;
    uint64_t geometryShaderInvocations;
    uint64_t geometryShaderPrimitives;
    uint64_t clippingInvocations;
    uint64_t clippingOutputPrimitives;
    uint64_t fragmentShaderInvocations;
    uint64_t tessellationControlShaderPatches;
    uint64_t tessellationEvaluationShaderInvocations;
    uint64_t computeShaderInvocations;
};


#endif
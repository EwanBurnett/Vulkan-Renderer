#include "../../include/Vulkan/VkInit.h"

#ifdef DEBUG
VkDebugUtilsMessengerCreateInfoEXT VKR::VkInit::MakeDebugUtilsMessengerCreateInfoEXT()
{
    return {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        nullptr,
        0,
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        nullptr,
        nullptr
    };
}

VkDebugReportCallbackCreateInfoEXT VKR::VkInit::MakeDebugReportCallbackCreateInfoEXT()
{
    return {
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        nullptr,
        VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT,
        nullptr,
        nullptr
    };
}

#endif


VkDeviceQueueCreateInfo VKR::VkInit::MakeDeviceQueueCreateInfo(const uint32_t queueFamilyIndex, const uint32_t numPriorities, const float* pPriorities)
{
    return {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         nullptr,
         0,
         queueFamilyIndex,
         numPriorities,
         pPriorities
    };

}

VkBufferCreateInfo VKR::VkInit::MakeBufferCreateInfo(const uint64_t size, const VkBufferUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices, const uint32_t flags) {
    return {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        flags,
        size,
        usage,
        sharingMode,
        queueFamilyIndexCount,
        pQueueFamilyIndices
    };
}


VkImageCreateInfo VKR::VkInit::MakeImageCreateInfo(const VkExtent3D extents, const VkImageType type, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices, const uint32_t flags)
{
    return {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        nullptr,
        0,
        type,
        format,
        extents,
        1,  //Mip Levels
        1,  //Array Layers
        VK_SAMPLE_COUNT_1_BIT,
        tiling,
        usage,
        sharingMode,
        queueFamilyIndexCount,
        pQueueFamilyIndices,
        VK_IMAGE_LAYOUT_UNDEFINED
    };
}

VkImageViewCreateInfo VKR::VkInit::MakeImageViewCreateInfo(VkImage image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags)
{
    return {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        nullptr,
        0,
        image,
        type,
        format,
        {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },
        {
            aspectFlags,
            0,
            1,
            0,
            1
        }
    };
}


VkPipelineVertexInputStateCreateInfo VKR::VkInit::MakePipelineVertexInputStateCreateInfo() {
    return {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        nullptr,
        0,
        0,
        nullptr,
        0,
        nullptr,
    };
}

VkPipelineInputAssemblyStateCreateInfo VKR::VkInit::MakePipelineInputAssemblyStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_FALSE,
    };
}

VkPipelineTessellationStateCreateInfo VKR::VkInit::MakePipelineTessellationStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        nullptr,
        0,
        0
    };
}

VkPipelineViewportStateCreateInfo VKR::VkInit::MakePipelineViewportStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        nullptr,
        0,
        0,
        nullptr,
        0,
        nullptr
    };
}

VkPipelineRasterizationStateCreateInfo VKR::VkInit::MakePipelineRasterizationStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE,
        VK_FALSE,
        VK_POLYGON_MODE_FILL,
        VK_CULL_MODE_NONE,
        VK_FRONT_FACE_CLOCKWISE,
        VK_FALSE,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    };
}

VkPipelineMultisampleStateCreateInfo VKR::VkInit::MakePipelineMultisampleStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FALSE,
        1.0f,
        nullptr,
        VK_FALSE,
        VK_FALSE
    };
}

VkPipelineDepthStencilStateCreateInfo VKR::VkInit::MakePipelineDepthStencilStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_TRUE,
        VK_TRUE,
        VK_COMPARE_OP_LESS,
        VK_FALSE,
        VK_TRUE,
        {},
        {},
        0.0f,
        1.0f,
    };
}

VkPipelineColorBlendStateCreateInfo VKR::VkInit::MakePipelineColorBlendStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE,
        VK_LOGIC_OP_COPY,
        0,
        nullptr,
        {
            0.0f,
            0.0f,
            0.0f,
            0.0f
        }
    };
}

VkPipelineDynamicStateCreateInfo VKR::VkInit::MakePipelineDynamicStateCreateInfo()
{
    return {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        nullptr,
        0,
        0,
        nullptr,
    };
}

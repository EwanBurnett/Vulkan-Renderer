#ifndef __VKRENDERER_VKINIT_H
#define __VKRENDERER_VKINIT_H
/**
*   @file VkInit.h
*   @brief Vulkan Structure Initializers
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/24
*/
#include "VkCommon.h"

namespace VKR {
    namespace VkInit {
#ifdef DEBUG
        VkDebugUtilsMessengerCreateInfoEXT MakeDebugUtilsMessengerCreateInfoEXT();
        VkDebugReportCallbackCreateInfoEXT MakeDebugReportCallbackCreateInfoEXT();
#endif

        VkImageViewCreateInfo MakeImageViewCreateInfo(VkImage image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags);

        VkDeviceQueueCreateInfo MakeDeviceQueueCreateInfo(const uint32_t queueFamilyIndex, const uint32_t numPriorities, const float* pPriorities);

        VkBufferCreateInfo MakeBufferCreateInfo(const uint64_t size, const VkBufferUsageFlags usage, const VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, const uint32_t queueFamilyIndexCount = 0, const uint32_t* pQueueFamilyIndices = nullptr, const uint32_t flags = 0);
        VkImageCreateInfo MakeImageCreateInfo(const VkExtent3D extents, const VkImageType type, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkSharingMode = VK_SHARING_MODE_EXCLUSIVE, const uint32_t queueFamilyIndexCount = 0, const uint32_t* pQueueFamilyIndices = nullptr, const uint32_t flags = 0);

        VkPipelineVertexInputStateCreateInfo MakePipelineVertexInputStateCreateInfo();
        VkPipelineInputAssemblyStateCreateInfo MakePipelineInputAssemblyStateCreateInfo();
        VkPipelineViewportStateCreateInfo MakePipelineViewportStateCreateInfo();
        VkPipelineRasterizationStateCreateInfo MakePipelineRasterizationStateCreateInfo(); 
        VkPipelineMultisampleStateCreateInfo MakePipelineMultisampleStateCreateInfo();
        VkPipelineDepthStencilStateCreateInfo MakePipelineDepthStencilStateCreateInfo(); 
        VkPipelineColorBlendStateCreateInfo MakePipelineColorBlendStateCreateInfo(); 
        VkPipelineDynamicStateCreateInfo MakePipelineDynamicStateCreateInfo(); 
    }

}

#endif
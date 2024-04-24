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
    }
}

#endif
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


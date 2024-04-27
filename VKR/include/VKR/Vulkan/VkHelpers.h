#ifndef __VKRENDERER_VKHELPERS_H
#define __VKRENDERER_VKHELPERS_H
/**
*   @file VkHelpers.h
*   @brief Vulkan Miscellaneous Helpers
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/24
*/

#include "VkCommon.h"
#include <vector> 

namespace VKR {
    namespace VkHelpers {
        uint32_t FindQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlags flags);

        uint32_t FindMemoryTypeIndex(VkPhysicalDevice device, uint32_t flags, VkMemoryPropertyFlags properties);

        VkFormat FindSupportedFormat(VkPhysicalDevice device, const uint32_t numFormats, const VkFormat* pFormats, VkImageTiling tiling, VkFormatFeatureFlags flags);
        VkFormat FindDepthFormat(VkPhysicalDevice device);
        bool ValidateStencilComponent(VkFormat format);

        bool ValidateInstanceExtensionSupport(const char* const extension, const uint32_t extensionPropertyCount, VkExtensionProperties* pExtensionProperties);
        bool ValidateInstanceExtensionSupportArray(const uint32_t numExtensions, const char* const* pExtensions);
        bool ValidateInstanceLayerSupport(const char* const layer, const uint32_t layerPropertyCount, VkLayerProperties* pLayerProperties);
        bool ValidateInstanceLayerSupportArray(const uint32_t numLayers, const char* const* pLayers);

        bool ValidatePhysicalDeviceExtensionSupport(const VkPhysicalDevice& device, const char* const extension, const uint32_t extensionPropertyCount, VkExtensionProperties* pExtensionProperties);
        bool ValidatePhysicalDeviceExtensionSupportArray(const VkPhysicalDevice& device, const uint32_t numExtensions, const char* const* pExtensions);

        bool ValidatePhysicalDevicePresentationSupport(const VkPhysicalDevice& device, const uint32_t presentQueueFamilyIndex, VkSurfaceKHR surface);

     
        VkResult QueryPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice device, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR& capabilities);
        std::vector<VkSurfaceFormatKHR> QueryPhysicalDeviceSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface);
        std::vector<VkPresentModeKHR> QueryPhysicalDeviceSurfacePresentModes(VkPhysicalDevice device, VkSurfaceKHR surface);

    }
}

#endif
#ifndef __VKRENDERER_VKCOMMON_H
#define __VKRENDERER_VKCOMMON_H
/**
*   @file VkCommon.h
*   @brief Vulkan Common Includes / utilites
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/24
*/


#include <Vulkan/vulkan.h>
#include "../Types.h"

namespace VKR {
    const VkResult VK_CHECK_IMPL(const VkResult result, const char* file, const uint64_t line, const char* function);
}

#define VK_CHECK(result) VKR::VK_CHECK_IMPL(result, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#endif

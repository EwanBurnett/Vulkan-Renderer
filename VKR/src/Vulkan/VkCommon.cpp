#include "../../include/VKR/Vulkan/VkCommon.h"
#include "../../include/VKR/Logger.h"
#include <Vulkan/vk_enum_string_helper.h>

const VkResult VKR::VK_CHECK_IMPL(const VkResult result, const char* file, const uint64_t line, const char* function)
{
#if DEBUG   //Only perform error logging in Debug builds
    if (result != VK_SUCCESS) {
        Log::Warning("[Vulkan]\t%s\n\tFILE: %s\n\tLINE: %d\n\tFUNCTION: %s", string_VkResult(result), file, line, function);
    }
#endif
    return result;  //Forward the initial VkResult to the caller
}

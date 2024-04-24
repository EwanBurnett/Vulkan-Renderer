#include "../../include/Vulkan/VkContext.h"
#include "../../include/Vulkan/VkInit.h"
#include "../../include/Vulkan/VkHelpers.h"
#include "../include/Logger.h"
#include <assert.h>
#include <easy/profiler.h>

VKR::VkContext::VkContext()
{
#ifdef DEBUG
    m_DebugLogger = VK_NULL_HANDLE; 
    m_DebugReporter = VK_NULL_HANDLE; 
#endif
    m_Instance = VK_NULL_HANDLE; 
}


#ifdef DEBUG
VKAPI_ATTR VkBool32 VKAPI_CALL VKR::VkContext::DebugLog(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    //Log warnings and errors to stdout. 
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        Log::Warning("[Vulkan]\t%s\n\n", pCallbackData->pMessage);
    }

    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\t%s\n\n", pCallbackData->pMessage);    //TODO: Retrieve from pUserData.
    }
    else {
        Log::Debug("[Vulkan]\t%s\n\n", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VKR::VkContext::DebugReport(VkDebugReportFlagBitsEXT reportFlags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, uint32_t code, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    //Ignore Warning Messages.
    if (reportFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        return VK_FALSE;
    }

    Log::Debug("[Vulkan]\t<%s>:\n\t%s", pLayerPrefix, pMessage);
    return VK_FALSE;
}

VkResult VKR::VkContext::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    //Load the function from the Vulkan Instance
    const PFN_vkCreateDebugUtilsMessengerEXT pfn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");

    if (pfn != nullptr) {
        return pfn(m_Instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VkResult VKR::VkContext::DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT& debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    const PFN_vkDestroyDebugUtilsMessengerEXT pfn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
    if (pfn != nullptr) {
        pfn(m_Instance, debugMessenger, pAllocator);
        return VK_SUCCESS;
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


VkResult VKR::VkContext::CreateDebugLogger()
{
#ifdef DEBUG
    VkDebugUtilsMessengerCreateInfoEXT createInfo = VkInit::MakeDebugUtilsMessengerCreateInfoEXT();
    createInfo.pfnUserCallback = &VkContext::DebugLog;
    createInfo.pUserData = this;

    return CreateDebugUtilsMessengerEXT(&createInfo, nullptr, &m_DebugLogger);
#else
    return VK_SUCCESS;
#endif
}

void VKR::VkContext::DestroyDebugLogger()
{
#ifdef DEBUG
    DestroyDebugUtilsMessengerEXT(m_DebugLogger, nullptr);
#endif
}

VkResult VKR::VkContext::CreateDebugReportCallbackEXT(const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pDebugReporter)
{
    const PFN_vkCreateDebugReportCallbackEXT pfn = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugReportCallbackEXT");

    if (pfn != nullptr) {
        return pfn(m_Instance, pCreateInfo, pAllocator, pDebugReporter);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VkResult VKR::VkContext::DestroyDebugReportCallbackEXT(VkDebugReportCallbackEXT& debugReporter, const VkAllocationCallbacks* pAllocator)
{
    const PFN_vkDestroyDebugReportCallbackEXT pfn = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");
    if (pfn != nullptr) {
        pfn(m_Instance, debugReporter, pAllocator);
        return VK_SUCCESS;
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VkResult VKR::VkContext::CreateDebugReporter()
{
#ifdef DEBUG
    VkDebugReportCallbackCreateInfoEXT createInfo = VkInit::MakeDebugReportCallbackCreateInfoEXT(); 
    createInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)&VkContext::DebugReport;
    createInfo.pUserData = this;

    return CreateDebugReportCallbackEXT(&createInfo, nullptr, &m_DebugReporter);

#else
    return VK_SUCCESS;
#endif
}

void VKR::VkContext::DestroyDebugReporter()
{
#ifdef DEBUG
    DestroyDebugReportCallbackEXT(m_DebugReporter, nullptr);
#endif
}

VkResult VKR::VkContext::DebugSetObjectName(uint64_t objectHandle, VkObjectType objectType, const char* name)
{
#ifdef DEBUG
    const VkDebugUtilsObjectNameInfoEXT nameInfo = {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        nullptr,
        objectType,
        objectHandle,
        name
    };

    PFN_vkSetDebugUtilsObjectNameEXT pfn = (PFN_vkSetDebugUtilsObjectNameEXT)(vkGetInstanceProcAddr(m_Instance, "vkSetDebugUtilsObjectNameEXT"));
    if (pfn != nullptr) {
        return pfn(m_Device, &nameInfo);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

#else
    return VK_SUCCESS;
#endif
}

#endif


const VkInstance& VKR::VkContext::GetInstance() const
{
    //Validate the instance before returning 
    if (m_Instance == VK_NULL_HANDLE) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tRequested VkInstance was VK_NULL_HANDLE! Was CreateInstance() called?");
    }
    return m_Instance; 
}

VkResult VKR::VkContext::CreateInstance(const uint32_t numLayers, const char* const* ppLayers, const uint32_t numExtensions, const char* const* ppExtensions, const VkApplicationInfo* pApplicationInfo)
{
    EASY_FUNCTION(profiler::colors::Red500);

    //Validate all requested instance layers and extensions
    if (!VkHelpers::ValidateInstanceLayerSupportArray(numLayers, ppLayers)) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tNot all layers were supported!\n");
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    if (!VkHelpers::ValidateInstanceExtensionSupportArray(numExtensions, ppExtensions)) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tNot all Extensions were supported!\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    //Link a Debug Messenger in Debug builds
#ifdef DEBUG
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo = VkInit::MakeDebugUtilsMessengerCreateInfoEXT(); 
    messengerInfo.pfnUserCallback = &VkContext::DebugLog;
    messengerInfo.pUserData = this;
#endif

    //Create the Instance
    const VkInstanceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef DEBUG
        &messengerInfo,
#else
        nullptr,
#endif
        0,
        pApplicationInfo,
        numLayers,
        ppLayers,
        numExtensions,
        ppExtensions
    };

    return vkCreateInstance(&createInfo, nullptr, &m_Instance);
}

void VKR::VkContext::DestroyInstance()
{
    EASY_FUNCTION(profiler::colors::Red500);
    vkDestroyInstance(m_Instance, nullptr);
}

const VkPhysicalDevice& VKR::VkContext::GetPhysicalDevice() const
{
    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tRequested VkPhysicalDevice was VK_NULL_HANDLE! Was SelectPhysicalDevice() called?");
    }
    return m_PhysicalDevice; 
}

VkResult VKR::VkContext::SelectPhysicalDevice() 
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(m_Instance, &count, nullptr);
    VkPhysicalDevice* pDevices = new VkPhysicalDevice[count];
    vkEnumeratePhysicalDevices(m_Instance, &count, pDevices);

    if (count == 0) {
        delete[] pDevices;
        return VK_ERROR_DEVICE_LOST;
    }

    m_PhysicalDevice = pDevices[0]; //TODO: Better Physical Device Selection
    delete[] pDevices;

    return VK_SUCCESS;
}

const VkDevice& VKR::VkContext::GetDevice() const
{
    if (m_Device == VK_NULL_HANDLE) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tRequested VkDevice was VK_NULL_HANDLE! Was CreateDevice() called?");
    }
    return m_Device; 
}

VkResult VKR::VkContext::CreateDevice(const uint32_t numExtensions, const char* const* ppExtensions, const uint32_t numQueues, const VkDeviceQueueCreateInfo* pQueueCreateInfos, const VkPhysicalDeviceFeatures* pFeatures)
{
    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        Log::Fatal(__FILE__, __LINE__, __PRETTY_FUNCTION__, true, "[Vulkan]\tPhysical Device was VK_NULL_HANDLE!\n");
        return VK_ERROR_UNKNOWN;
    }

    //Validate each requested extension
    if (!VkHelpers::ValidatePhysicalDeviceExtensionSupportArray(m_PhysicalDevice, numExtensions, ppExtensions)) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tNot all Extensions were supported!\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    const VkDeviceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        numQueues,
        pQueueCreateInfos,
        0,
        nullptr,
        numExtensions,
        ppExtensions,
        pFeatures
    };

    return vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
}

void VKR::VkContext::DestroyDevice()
{
    vkDestroyDevice(m_Device, nullptr);
}

VkQueue VKR::VkContext::GetDeviceQueue(const uint32_t queueFamilyIndex, const uint32_t queueIndex)
{
    VkQueue queue = VK_NULL_HANDLE; 
    vkGetDeviceQueue(m_Device, queueFamilyIndex, queueIndex, &queue);
    return queue; 
}

VkResult VKR::VkContext::CreateSemaphore(VkSemaphore* pSemaphore)
{
    const VkSemaphoreCreateInfo createInfo = {
       VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
       nullptr,
       0
    };

    return vkCreateSemaphore(m_Device, &createInfo, nullptr, pSemaphore);
}

void VKR::VkContext::DestroySemaphore(VkSemaphore& semaphore)
{
    vkDestroySemaphore(m_Device, semaphore, nullptr);
}

VkResult VKR::VkContext::CreateFence(VkFence* pFence)
{
    VkFenceCreateInfo createInfo = {
       VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
       nullptr,
       VK_FENCE_CREATE_SIGNALED_BIT
    };

    return vkCreateFence(m_Device, &createInfo, nullptr, pFence);
}

void VKR::VkContext::DestroyFence(VkFence& fence)
{
    vkDestroyFence(m_Device, fence, nullptr);
}

VkResult VKR::VkContext::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* pImageView) const
{
    const VkImageViewCreateInfo createInfo = VkInit::MakeImageViewCreateInfo(image, VK_IMAGE_VIEW_TYPE_2D, format, aspectFlags);

    return vkCreateImageView(m_Device, &createInfo, nullptr, pImageView); 
}

void VKR::VkContext::DestroyImageView(VkImageView& imageView) const
{
    vkDestroyImageView(m_Device, imageView, nullptr);
}

VkResult VKR::VkContext::CreateCommandPool(const uint32_t queueFamilyIndex, const uint32_t flags, VkCommandPool* pCommandPool)
{
    const VkCommandPoolCreateInfo createInfo = {
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      nullptr,
      flags,
      queueFamilyIndex
    };

    return vkCreateCommandPool(m_Device, &createInfo, nullptr, pCommandPool);
}

void VKR::VkContext::DestroyCommandPool(VkCommandPool& commandPool)
{
    vkDestroyCommandPool(m_Device, commandPool, nullptr);
}
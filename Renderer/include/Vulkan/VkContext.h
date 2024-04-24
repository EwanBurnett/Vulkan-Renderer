#ifndef __VKRENDERER_VKCONTEXT_H
#define __VKRENDERER_VKCONTEXT_H
/**
*   @file VkContext.h
*   @brief Vulkan GPU Context
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/24
*/

#include "VkCommon.h"

namespace VKR {
    class VkContext {
    public: 
        VkContext(); 

        //Instance Level Functions
#ifdef DEBUG
        VkResult CreateDebugLogger();
        void DestroyDebugLogger();

        VkResult CreateDebugReporter();
        void DestroyDebugReporter();

        VkResult DebugSetObjectName(uint64_t objectHandle, VkObjectType objectType, const char* name);
#endif

        const VkInstance& GetInstance() const; 
        VkResult CreateInstance(const uint32_t numLayers, const char* const* ppLayers, const uint32_t numExtensions, const char* const* ppExtensions, const VkApplicationInfo* pApplicationInfo = nullptr);
        void DestroyInstance(); 


    private:
#ifdef DEBUG
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugLog(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReport(VkDebugReportFlagBitsEXT reportFlags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, uint32_t code, const char* pLayerPrefix, const char* pMessage, void* pUserData);

        VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        VkResult DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT& debugMessenger, const VkAllocationCallbacks* pAllocator);

        VkResult CreateDebugReportCallbackEXT(const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pDebugReporter);
        VkResult DestroyDebugReportCallbackEXT(VkDebugReportCallbackEXT& debugReporter, const VkAllocationCallbacks* pAllocator);
#endif


    private:
        VkInstance m_Instance; 
        VkPhysicalDevice m_PhysicalDevice; 
        VkDevice m_Device; 
#ifdef DEBUG
        VkDebugUtilsMessengerEXT m_DebugLogger; 
        VkDebugReportCallbackEXT m_DebugReporter; 
#endif
    };
}

#endif
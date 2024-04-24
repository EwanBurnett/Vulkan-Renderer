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


        //Physical Device
        const VkPhysicalDevice& GetPhysicalDevice() const; 
        VkResult SelectPhysicalDevice(); 

        //Logical Device
        const VkDevice& GetDevice() const; 
        VkResult CreateDevice(const uint32_t numExtensions, const char* const* ppExtensions, const uint32_t numQueues, const VkDeviceQueueCreateInfo* pQueueCreateInfos, const VkPhysicalDeviceFeatures* pFeatures = nullptr);
        void DestroyDevice(); 

        //VMA
        VkResult CreateAllocator(); 
        void DestroyAllocator(); 

        //Device Level Functions
        VkQueue GetDeviceQueue(const uint32_t queueFamilyIndex, const uint32_t queueIndex);

        VkResult CreateSemaphore(VkSemaphore* pSemaphore);
        void DestroySemaphore(VkSemaphore& semaphore);

        VkResult CreateFence(VkFence* pFence, bool startSignaled = true);
        void DestroyFence(VkFence& fence);

        VkResult CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, const VmaMemoryUsage memoryUsage, const uint32_t memoryFlags, VmaAllocation* pAllocation, VkBuffer* pBuffer);
        void DestroyBuffer(VkBuffer& buffer, VmaAllocation& allocation);

        VkResult CreateImage(const VkImageType type, const VkExtent3D extents, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, const VmaMemoryUsage memoryUsage, const uint32_t memoryFlags, VmaAllocation*  pAllocation, VkImage* pImage);
        void DestroyImage(VkImage& image, VmaAllocation& allocation);

        VkResult CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* pImageView) const;
        void DestroyImageView(VkImageView& imageView) const;

        VkResult CreateCommandPool(const uint32_t queueFamilyIndex, const uint32_t flags, VkCommandPool* pCommandPool);
        void DestroyCommandPool(VkCommandPool& commandPool);

        VkResult AllocateCommandBuffers(VkCommandPool commandPool, VkCommandBufferLevel level, const uint32_t count, VkCommandBuffer* pCommandBuffers);

        VkResult CreateSampler(VkSampler* pSampler);
        void DestroySampler(VkSampler& sampler);

        VkResult CreateDescriptorPool(const uint32_t uniformBufferCount, const uint32_t storageBufferCount, const uint32_t samplerCount, VkDescriptorPool* pDescriptorPool);
        void DestroyDescriptorPool(VkDescriptorPool& descriptorPool);


        VkResult CreatePipelineLayout(const uint32_t numDescriptors, VkDescriptorSetLayout* pDescriptors, VkPipelineLayout* pPipelineLayout);
        void DestroyPipelineLayout(VkPipelineLayout pipelineLayout);

        VkResult CreateRenderPass(const uint32_t numAttachments, const VkAttachmentDescription* pAttachments, const uint32_t numSubpasses, const VkSubpassDescription* pSubpasses, const uint32_t numDependencies, const VkSubpassDependency* pDependencies, VkRenderPass* pRenderPass);
        void DestroyRenderPass(VkRenderPass& renderPass);


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
        VmaAllocator m_Allocator; 
    };
}

#endif
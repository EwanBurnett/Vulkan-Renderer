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
#ifdef VKR_DEBUG
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

        VkResult Map(const VmaAllocation& allocation, void** ppData) const;
        void Unmap(const VmaAllocation& allocation) const;

        //Device Level Functions
        VkQueue GetDeviceQueue(const uint32_t queueFamilyIndex, const uint32_t queueIndex) const;

        VkResult CreateSemaphore(VkSemaphore* pSemaphore) const;
        void DestroySemaphore(VkSemaphore& semaphore) const;

        VkResult CreateFence(VkFence* pFence, bool startSignaled = true) const;
        void DestroyFence(VkFence& fence) const;

        VkResult CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, const VmaMemoryUsage memoryUsage, const uint32_t memoryFlags, VmaAllocation* pAllocation, VkBuffer* pBuffer) const;
        void DestroyBuffer(VkBuffer& buffer, VmaAllocation& allocation) const;

        VkResult CreateImage(const VkImageType type, const VkExtent3D extents, const VkSampleCountFlagBits sampleCount, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, const VmaMemoryUsage memoryUsage, const uint32_t memoryFlags, VmaAllocation* pAllocation, VkImage* pImage) const;
        void DestroyImage(VkImage& image, VmaAllocation& allocation) const;

        VkResult CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* pImageView) const;
        void DestroyImageView(VkImageView& imageView) const;

        VkResult CreateShaderModule(const char* const pBlob, const size_t byteWidth, VkShaderModule* pShaderModule) const;
        void DestroyShaderModule(VkShaderModule& shaderModule) const;

        VkResult CreateSampler(VkSampler* pSampler) const;
        void DestroySampler(VkSampler& sampler) const;

        VkResult CreateCommandPool(const uint32_t queueFamilyIndex, const uint32_t flags, VkCommandPool* pCommandPool) const;
        void DestroyCommandPool(VkCommandPool& commandPool) const;

        VkResult AllocateCommandBuffers(const VkCommandPool commandPool, VkCommandBufferLevel level, const uint32_t count, VkCommandBuffer* pCommandBuffers) const;
        void FreeCommandBuffers(const VkCommandPool commandPool, const uint32_t count, VkCommandBuffer* pCommandBuffers) const;

        VkResult CreateDescriptorPool(const uint32_t maxSets, const uint32_t flags, const uint32_t numPoolSizes, const VkDescriptorPoolSize* pPoolSizes, VkDescriptorPool* pDescriptorPool) const;
        void DestroyDescriptorPool(VkDescriptorPool& descriptorPool) const;

        VkResult CreateDescriptorSetLayout(const uint32_t numBindings, const VkDescriptorSetLayoutBinding* pBindings, VkDescriptorSetLayout* pLayout);
        void DestroyDescriptorSetlayout(VkDescriptorSetLayout& layout);
        
        VkResult AllocateDescriptorSets(const VkDescriptorPool descriptorPool, const uint32_t numSets, const VkDescriptorSetLayout* pLayouts, VkDescriptorSet* pSets);

        VkResult CreatePipelineLayout(const uint32_t numDescriptors, const VkDescriptorSetLayout * pDescriptors, const uint32_t numPushConstants, const VkPushConstantRange* pPushConstants, VkPipelineLayout* pPipelineLayout) const;
        void DestroyPipelineLayout(VkPipelineLayout& pipelineLayout) const;

        VkResult CreateComputePipelines(const uint32_t numPipelines, const VkComputePipelineCreateInfo* pCreateInfos, const VkPipelineCache pipelineCache, VkPipeline* pPipelines) const;
        VkResult CreateGraphicsPipelines(const uint32_t numPipelines, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkPipelineCache pipelineCache, VkPipeline* pPipelines) const;
        void DestroyPipeline(VkPipeline& pipeline) const;

        VkResult CreateRenderPass(const uint32_t numAttachments, const VkAttachmentDescription* pAttachments, const uint32_t numSubpasses, const VkSubpassDescription* pSubpasses, const uint32_t numDependencies, const VkSubpassDependency* pDependencies, VkRenderPass* pRenderPass) const;
        void DestroyRenderPass(VkRenderPass& renderPass) const;

        VkResult CreateFrameBuffer(const VkExtent3D extents, const VkRenderPass renderPass, const uint32_t numAttachments, const VkImageView* pAttachments, VkFramebuffer* pFrameBuffer) const;
        void DestroyFrameBuffer(VkFramebuffer& frameBuffer) const;

        VkResult CreateQueryPool(const VkQueryType type, const uint32_t count, const VkQueryPipelineStatisticFlags pipelineStatistics, VkQueryPool* pPool);
        void DestroyQueryPool(VkQueryPool& pool);

    private:
#ifdef VKR_DEBUG
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
#ifdef VKR_DEBUG
        VkDebugUtilsMessengerEXT m_DebugLogger;
        VkDebugReportCallbackEXT m_DebugReporter;
#endif
        VmaAllocator m_Allocator;
    };
}

#endif
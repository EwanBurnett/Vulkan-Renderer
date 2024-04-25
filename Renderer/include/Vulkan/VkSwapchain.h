#ifndef __VKRENDERER_VKSWAPCHAIN_H
#define __VKRENDERER_VKSWAPCHAIN_H
/**
*   @file VkContext.h
*   @brief Vulkan GPU Context
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/24
*/
#include "VkCommon.h"
#include <vector> 

namespace VKR {
    class VkContext;
    class Window;

    class VkSwapchain {
    public:
        VkResult Create(const VkContext& context, const Window* pWindow, const uint32_t queueFamilyIndex);
        void Destroy(const VkContext& context);

        const VkSwapchainKHR& GetSwapchain() const;

        const uint32_t GetImageCount() const;
        const std::vector<VkImage>& GetImages() const;
        const std::vector<VkImageView>& GetImageViews() const;


        VkResult SetClearValue(float r, float g, float b, float a, float depth, uint32_t stencil);
        VkClearValue GetColourClearValue() const;
        VkClearValue GetDepthStencilClearValue() const;

        VkResult Present(VkQueue queue, VkSemaphore semaphore, uint32_t* pIndex);

    private:
        VkResult CreateSurfaceKHR(const VkInstance& device, const Window* pWindow);
        void DestroySurfaceKHR(const VkInstance& instance);

        VkResult CreateSwapchainKHR(const VkDevice& device, const uint32_t imageCount, const uint32_t width, const uint32_t height, const uint32_t queueFamily);
        void DestroySwapchainKHR(const VkDevice& device);

        const uint32_t SelectImageCount();
        VkResult AcquireSwapchainImages(const VkContext& context);
        VkPresentModeKHR SelectPresentMode(const uint64_t numModes, const VkPresentModeKHR* pModes);
        VkSurfaceFormatKHR SelectSurfaceFormat(const uint64_t numFormats, const VkSurfaceFormatKHR* pFormats);


    private:
        VkSwapchainKHR m_Swapchain;
        VkSurfaceKHR m_Surface;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;

        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        VkSurfaceFormatKHR m_SurfaceFormat;
        VkPresentModeKHR m_SurfacePresentMode;

        VkClearValue m_ColourClearValue;
        VkClearValue m_DepthStencilClearValue;
    };
};


#endif
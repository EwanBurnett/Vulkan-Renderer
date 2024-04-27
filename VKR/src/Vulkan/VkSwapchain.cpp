#include "../../include/VKR/Vulkan/VkSwapchain.h"
#include "../../include/VKR/Vulkan/VkContext.h"
#include "../../include/VKR/Vulkan/VkInit.h"
#include "../../include/VKR/Vulkan/VkHelpers.h"
#include "../include/VKR/Window.h"
#include "../include/VKR/Logger.h"
#include <assert.h>
#include <easy/profiler.h>

VkResult VKR::VkSwapchain::Create(const VkContext& context, const Window* pWindow, const uint32_t queueFamilyIndex)
{
    EASY_FUNCTION(profiler::colors::Red500);
    CreateSurfaceKHR(context.GetInstance(), pWindow);

    //Query Physical Device Swapchain Support
    VkHelpers::QueryPhysicalDeviceSurfaceCapabilitiesKHR(context.GetPhysicalDevice(), m_Surface, m_SurfaceCapabilities);
    std::vector<VkSurfaceFormatKHR> formats = VkHelpers::QueryPhysicalDeviceSurfaceFormats(context.GetPhysicalDevice(), m_Surface);
    std::vector<VkPresentModeKHR> modes = VkHelpers::QueryPhysicalDeviceSurfacePresentModes(context.GetPhysicalDevice(), m_Surface);

    m_SurfaceFormat = SelectSurfaceFormat(formats.size(), formats.data());
    m_SurfacePresentMode = SelectPresentMode(modes.size(), modes.data());

    uint32_t imageCount = SelectImageCount();

    CreateSwapchainKHR(context.GetDevice(), imageCount, pWindow->GetWidth(), pWindow->GetHeight(), queueFamilyIndex);

    AcquireSwapchainImages(context);

    SetClearValue(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0x00);
    return VK_SUCCESS;
}

void VKR::VkSwapchain::Destroy(const VkContext& context)
{
    for (uint32_t i = 0; i < m_ImageViews.size(); i++) {
        context.DestroyImageView(m_ImageViews[i]);
    }

    DestroySwapchainKHR(context.GetDevice());
    DestroySurfaceKHR(context.GetInstance());
}

const VkSwapchainKHR& VKR::VkSwapchain::GetSwapchain() const
{
    if (m_Swapchain == VK_NULL_HANDLE) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tRequested VkSwapchain was VK_NULL_HANDLE! Was CreateSwapchain() called?");
    }
    return m_Swapchain;
}

const uint32_t VKR::VkSwapchain::GetImageCount() const
{
    return m_Images.size(); 
}

const std::vector<VkImage>& VKR::VkSwapchain::GetImages() const
{
    return m_Images; 
}

const std::vector<VkImageView>& VKR::VkSwapchain::GetImageViews() const
{
    return m_ImageViews;
}

void VKR::VkSwapchain::SetClearValue(float r, float g, float b, float a, float depth, uint32_t stencil)
{
    m_ColourClearValue.color = { r, g, b, a };
    m_DepthStencilClearValue.depthStencil = { depth, stencil };
}


VkClearValue VKR::VkSwapchain::GetDepthStencilClearValue() const
{
    return m_DepthStencilClearValue; 
}

VkClearValue VKR::VkSwapchain::GetColourClearValue() const
{
    return m_ColourClearValue;
}

VkResult VKR::VkSwapchain::Present(VkQueue queue, VkSemaphore semaphore, uint32_t* pIndex)
{
    EASY_FUNCTION(profiler::colors::Red500);

    const VkPresentInfoKHR presentInfo = { //TODO: VkInit
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        1,
        &semaphore,
        1,
        &m_Swapchain,
        pIndex,
        nullptr
    };

    return vkQueuePresentKHR(queue, &presentInfo);
}

VkResult VKR::VkSwapchain::CreateSurfaceKHR(const VkInstance& instance, const Window* pWindow)
{
    EASY_FUNCTION(profiler::colors::Red500);
    //We'll dispatch to GLFW for this, as it's (mostly) platform independent. 
    return glfwCreateWindowSurface(instance, pWindow->GLFWHandle(), nullptr, &m_Surface);
}

void VKR::VkSwapchain::DestroySurfaceKHR(const VkInstance& instance)
{
    EASY_FUNCTION(profiler::colors::Red500);
    vkDestroySurfaceKHR(instance, m_Surface, nullptr);
}

VkResult VKR::VkSwapchain::CreateSwapchainKHR(const VkDevice& device, const uint32_t imageCount, const uint32_t width, const uint32_t height, const uint32_t queueFamily)
{
    EASY_FUNCTION(profiler::colors::Red500);
    const VkSwapchainCreateInfoKHR createInfo = {  
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        m_Surface,
        imageCount,
        m_SurfaceFormat.format,
        m_SurfaceFormat.colorSpace,
        {width, height},
        1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        1,
        &queueFamily,
        m_SurfaceCapabilities.currentTransform,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        m_SurfacePresentMode,
        VK_TRUE,
        nullptr
    };

    return vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_Swapchain);
}

void VKR::VkSwapchain::DestroySwapchainKHR(const VkDevice& device)
{
    EASY_FUNCTION(profiler::colors::Red500);
    vkDestroySwapchainKHR(device, m_Swapchain, nullptr);
}

const uint32_t VKR::VkSwapchain::SelectImageCount()
{
    const uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 1; //Request an additional image to the minimum to not wait on the GPU. 

    const bool imageCountExceeded = m_SurfaceCapabilities.maxImageCount && imageCount > m_SurfaceCapabilities.maxImageCount;
    return imageCountExceeded ? m_SurfaceCapabilities.maxImageCount : imageCount;
}

VkResult VKR::VkSwapchain::AcquireSwapchainImages(const VkContext& context)
{
    uint32_t imageCount = 0; 
    vkGetSwapchainImagesKHR(context.GetDevice(), m_Swapchain, &imageCount, nullptr);
    m_Images.resize(imageCount);
    m_ImageViews.resize(imageCount);
    vkGetSwapchainImagesKHR(context.GetDevice(), m_Swapchain, &imageCount, m_Images.data());

    for (uint32_t i = 0; i < imageCount; i++) {
        context.CreateImageView(m_Images[i], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &m_ImageViews[i]);
    }

    return VK_SUCCESS;
}

VkPresentModeKHR VKR::VkSwapchain::SelectPresentMode(const uint64_t numModes, const VkPresentModeKHR* pModes)
{
    for (uint64_t i = 0; i < numModes; i++) {
        if (pModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return pModes[i];
        }
    }

    //Fall back to FIFO
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR VKR::VkSwapchain::SelectSurfaceFormat(const uint64_t numFormats, const VkSurfaceFormatKHR* pFormats)
{
    //TODO: We're being lazy here again, and hard-coding a format. 
    return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
}


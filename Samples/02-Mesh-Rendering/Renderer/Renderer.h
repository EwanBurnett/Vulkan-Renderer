#ifndef __RENDERER_H
#define __RENDERER_H

#include "Helpers.h"
#include "GUI.h"

#include <VKR/Window.h>
#include <VKR/Vulkan/VkContext.h>
#include <VKR/Vulkan/VkSwapchain.h>

class Scene;

class Renderer {
public:
    void Init(const VKR::VkContext& context, const VKR::Window& window);
    void Shutdown(const VKR::VkContext& context);

    void BeginFrame(const VKR::VkContext& context);
    void EndFrame();

    void Draw(const Scene& scene);

private:
    void CreateSwapchain(const VKR::VkContext& context, const VKR::Window& window);
    void DestroySwapchain(const VKR::VkContext& context);

    void CreateColourRenderTarget(const VKR::VkContext& context, const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource);

    void CreateDepthRenderTarget(const VKR::VkContext& context, const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource);


    void DestroyImageResource(const VKR::VkContext& context, ImageResource& resource);
private:
    //Swapchain 
    VKR::VkSwapchain m_Swapchain;

    VkViewport m_Viewport;
    VkRect2D m_Scissor;

    VkSampleCountFlagBits m_MSAASamples;
    std::vector<ImageResource> m_RenderTargets;
    std::vector<VkFramebuffer> m_FrameBuffers;

    uint32_t m_ImageIndex; 

    VkQueue m_Queue;
    uint32_t m_QueueFamilyIndex;

    VkRenderPass m_RenderPass;


    VkSemaphore m_Semaphore; 


};
#endif
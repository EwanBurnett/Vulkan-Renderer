#include <DemoApp.h>
#include <vector> 
#include <VKR/Window.h>
#include <VKR/Timer.h>
#include <VKR/Vulkan/VkContext.h>
#include <VKR/Vulkan/VkSwapchain.h>
#include <VKR/Vulkan/VkimGui.h>

namespace Samples
{
    class HelloTriangleApp : public DemoApp {
    private:
        /**
         * @brief Wrapper for the resources required for images.
        */
        struct ImageResource {
            VkImage image;
            VmaAllocation allocation;
            VkImageView view;
        };

        /**
         * @brief Wrapper for buffer resources.
        */
        struct BufferResource {
            VkBuffer buffer;
            VmaAllocation allocation;
        };

        /**
         * @brief Storage class for Pipeline Statistics
        */
        struct PipelineStatistics
        {
            uint64_t inputAssemblyVertices;
            uint64_t inputAssemblyPrimitives;
            uint64_t vertexShaderInvocations;
            uint64_t geometryShaderInvocations;
            uint64_t geometryShaderPrimitives;
            uint64_t clippingInvocations;
            uint64_t clippingOutputPrimitives;
            uint64_t fragmentShaderInvocations;
            uint64_t tessellationControlShaderPatches;
            uint64_t tessellationEvaluationShaderInvocations;
            uint64_t computeShaderInvocations;
        };

    public:
        HelloTriangleApp();

        // Inherited via DemoApp
        virtual void Init(const VKR::Window& window) override;
        virtual void Update() override;
        virtual void FixedUpdate() override;
        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void Shutdown() override;

    private:
        void UpdateFrameCounter();
        void Synchronize();

        void DrawGUI();

        void CreateColourRenderTarget(const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource);
        void CreateDepthRenderTarget(const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource);

        void DestroyImageResource(ImageResource& resource);

        void CreateSwapchain(const VKR::Window& window, VkSampleCountFlagBits samples);

    private:
        VKR::Timer m_Timer;

        VKR::VkContext m_Context;
        VKR::VkSwapchain m_Swapchain;

        VkQueue m_Queue;
        uint32_t m_QueueFamilyIndex;

        uint64_t m_FrameInFlight;
        uint32_t m_ImageIndex;

        std::vector<VkFence> m_fFrameReady;
        std::vector<VkSemaphore> m_sImageAvailable;
        std::vector<VkSemaphore> m_sRenderFinished;

        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_Commands;

        VkRenderPass m_RenderPass;

        VkSampleCountFlagBits m_MSAASamples;
        std::vector<ImageResource> m_RenderTargets;
        std::vector<VkFramebuffer> m_FrameBuffers;

        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;

        VKR::VkImGui m_ImGuiRenderer;

        VkViewport m_Viewport;
        VkRect2D m_Scissor;

        VkQueryPool m_PipelineQueryPool;
        PipelineStatistics m_PipelineStatistics;
        VkQueryPool m_OcclusionQueryPool;
        uint64_t m_OcclusionStatistics;

        VkPhysicalDeviceProperties m_DeviceProperties; 
    };
}
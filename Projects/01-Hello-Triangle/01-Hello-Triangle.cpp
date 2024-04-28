/**
*   @file 01-Hello-Triangle.cpp
*   @brief Demo Project utilizing VKR to display a Triangle within a window.
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/28
*/

#include <DemoApp.h>
#include <vector> 

#include <VKR/VKR.h> 
#include <VKR/Window.h>
#include <VKR/Vulkan/VkContext.h>
#include <VKR/Vulkan/VkSwapchain.h>
#include <VKR/Vulkan/VkHelpers.h>
#include <VKR/Vulkan/VkimGui.h>
#include <VKR/Vulkan/VkPipelineBuilder.h>
#include <VKR/Vulkan/VkInit.h>
#include <VKR/File.h>
#include <VKR/Timer.h>
#include <VKR/Maths.h>
#include <VKR/Logger.h>
#include <easy/profiler.h>

constexpr uint32_t WINDOW_WIDTH = 600;
constexpr uint32_t WINDOW_HEIGHT = 400;

constexpr uint32_t FRAMES_IN_FLIGHT = 3;


using namespace VKR;




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

    void CreateSwapchain(const Window& window, VkSampleCountFlagBits samples);

private:
    Timer m_Timer;

    VkContext m_Context;
    VkSwapchain m_Swapchain;

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

    VkImGui m_ImGuiRenderer;
};


int main() {
    //Initialize the Application. 
    VKR::Init();

    //Create a Window. 
    Window window;
    window.Create("VKR Sample 01 - Hello Triangle", WINDOW_WIDTH, WINDOW_HEIGHT);

    HelloTriangleApp demoApp;
    demoApp.Init(window);

    //Run the Render Loop
    while (window.PollEvents()) {
        demoApp.BeginFrame();

        demoApp.Update();
        demoApp.FixedUpdate();

        demoApp.EndFrame();
    }
    Log::Print("\n");   //Print a newline for correct log output. 

    window.Hide();

    demoApp.Shutdown();
    window.Destroy();

    VKR::Shutdown();
    return 0;
}


HelloTriangleApp::HelloTriangleApp() : DemoApp()
{
    m_MSAASamples = VK_SAMPLE_COUNT_4_BIT;
}


void HelloTriangleApp::Init(const VKR::Window& window)
{
    EASY_FUNCTION();
    Log::Message("Initializing Application...\n");
    VKR::Timer initTimer;
    initTimer.Start();

    //Vulkan Instance Creation
    {
        const std::vector<const char*> instanceLayers = {
    #if DEBUG
           "VK_LAYER_KHRONOS_validation",
    #endif
        };

        std::vector<const char*> instanceExtensions = {
    #if DEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
    #endif
        };
        {
            uint32_t optExtCount = 0;
            auto ext = glfwGetRequiredInstanceExtensions(&optExtCount);
            for (int i = 0; i < optExtCount; i++) {
                instanceExtensions.push_back(ext[i]);
            }
        }

        VK_CHECK(m_Context.CreateInstance(instanceLayers.size(), instanceLayers.data(), instanceExtensions.size(), instanceExtensions.data(), nullptr));
    }

#if DEBUG
    //Vulkan Debug Callback Object Creation
    {
        m_Context.CreateDebugLogger();
        m_Context.CreateDebugReporter();
    }
#endif

    //Vulkan Device Creation
    {
        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    #if VKR_DEBUG
            VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
    #endif
        };

#if VKR_DEBUG
        VK_CHECK(m_Context.CreateDebugLogger());
        VK_CHECK(m_Context.CreateDebugReporter());
#endif

        VK_CHECK(m_Context.SelectPhysicalDevice());

        m_QueueFamilyIndex = VkHelpers::FindQueueFamilyIndex(m_Context.GetPhysicalDevice(), VK_QUEUE_GRAPHICS_BIT);
        float queuePriorities[] = { 1.0f };
        const VkDeviceQueueCreateInfo qci = VkInit::MakeDeviceQueueCreateInfo(0, 1, queuePriorities);

        VkPhysicalDeviceFeatures features = {};
        m_Context.CreateDevice(deviceExtensions.size(), deviceExtensions.data(), 1, &qci, &features);
    }

    m_Context.CreateAllocator();

    m_Queue = m_Context.GetDeviceQueue(m_QueueFamilyIndex, 0);


    //Create the Command Pool
    m_Context.CreateCommandPool(m_QueueFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, &m_CommandPool);

    m_Commands.resize(FRAMES_IN_FLIGHT);

    //Size our Synchronization objects appropriately to the number of frames we're processing in flight. 
    m_fFrameReady.resize(FRAMES_IN_FLIGHT);
    m_sImageAvailable.resize(FRAMES_IN_FLIGHT);
    m_sRenderFinished.resize(FRAMES_IN_FLIGHT);

    //Create our Synchronization objects through the VkContext.
    for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        m_Context.CreateFence(&m_fFrameReady[i], true); //Signal the fence on creation
        m_Context.CreateSemaphore(&m_sImageAvailable[i]);
        m_Context.CreateSemaphore(&m_sRenderFinished[i]);
    }


    m_ImGuiRenderer.Init(m_Context, window);
    CreateSwapchain(window, m_MSAASamples);

    //Start the application timer after resource initialization
    m_Timer.Start();

    initTimer.Tick();
    Log::Message("Application Initialization complete in %fms!\n", initTimer.Duration());
}

void HelloTriangleApp::Update()
{
    EASY_FUNCTION();

    //Begin a render pass
    {
        const VkClearValue clearValues[3] = {   //TODO: Wrap Render Targets in a struct, and include this. 
            m_Swapchain.GetColourClearValue(),
            m_Swapchain.GetDepthStencilClearValue(),
            m_Swapchain.GetColourClearValue()
        };

        VkRenderPassBeginInfo renderPassBeginInfo = {
            VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            nullptr,
            m_RenderPass,
            m_FrameBuffers[m_ImageIndex],
            {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
            3,
            clearValues
        };
        vkCmdBeginRenderPass(m_Commands[m_FrameInFlight], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        //Draw Stuff. 

        DrawGUI();

        vkCmdEndRenderPass(m_Commands[m_FrameInFlight]);
    }
}

void HelloTriangleApp::FixedUpdate()
{
    EASY_FUNCTION();
}

void HelloTriangleApp::BeginFrame()
{
    EASY_FUNCTION();
    UpdateFrameCounter();

    Synchronize();

    //Acquire the next swapchain image index. 
    vkAcquireNextImageKHR(m_Context.GetDevice(), m_Swapchain.GetSwapchain(), UINT64_MAX, m_sImageAvailable[m_FrameInFlight], VK_NULL_HANDLE, &m_ImageIndex);

    //Allocate a new Command Buffer for this frame. 
    m_Context.FreeCommandBuffers(m_CommandPool, 1, &m_Commands[m_FrameInFlight]);
    m_Context.AllocateCommandBuffers(m_CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &m_Commands[m_FrameInFlight]);

    //Begin recording into the new command buffer. 
    const VkCommandBufferBeginInfo beginInfo = {
               VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
               nullptr,
               VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
               nullptr
    };
    vkBeginCommandBuffer(m_Commands[m_FrameInFlight], &beginInfo);
}

void HelloTriangleApp::EndFrame()
{
    EASY_FUNCTION();


    //End command buffer recording
    vkEndCommandBuffer(m_Commands[m_FrameInFlight]);

    //Submit the command buffer to the queue. 
    {
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

        const VkSubmitInfo submitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            1,
            &m_sImageAvailable[m_FrameInFlight],
            waitStages,
            1,
            &m_Commands[m_FrameInFlight],
            1,
            &m_sRenderFinished[m_FrameInFlight]
        };


        vkQueueSubmit(m_Queue, 1, &submitInfo, m_fFrameReady[m_FrameInFlight]);
    }

    //Present the image to the screen. 
    m_Swapchain.Present(m_Queue, m_sRenderFinished[m_FrameInFlight], &m_ImageIndex);

    m_FrameCount++; //Update our internal frame counter. 
}

void HelloTriangleApp::Shutdown()
{
    EASY_FUNCTION();

    //Wait for all device work to be completed before attempting to destroy any objects. 
    vkDeviceWaitIdle(m_Context.GetDevice());

    //Destroy application resources
    m_ImGuiRenderer.Shutdown(m_Context);

    for (uint32_t i = 0; i < m_Swapchain.GetImageCount(); i++) {
        m_Context.DestroyFrameBuffer(m_FrameBuffers[i]);
    }

    m_Context.DestroyRenderPass(m_RenderPass);
    for (uint32_t i = 0; i < m_RenderTargets.size(); i++) {
        DestroyImageResource(m_RenderTargets[i]);
    }

    for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        m_Context.DestroyFence(m_fFrameReady[i]);
        m_Context.DestroySemaphore(m_sImageAvailable[i]);
        m_Context.DestroySemaphore(m_sRenderFinished[i]);
    }

    m_Context.DestroyCommandPool(m_CommandPool);

    //Destroy Vulkan Context and Swapchain
    m_Swapchain.Destroy(m_Context);

#if DEBUG
    m_Context.DestroyDebugReporter();
    m_Context.DestroyDebugLogger();
#endif

    m_Context.DestroyAllocator();
    m_Context.DestroyDevice();

    m_Context.DestroyInstance();
}

void HelloTriangleApp::UpdateFrameCounter()
{
    EASY_FUNCTION();

    m_Timer.Tick();

    //Retrieve current DeltaTime 
    m_DeltaTime = m_Timer.DeltaTime();
    m_FPS = static_cast<uint64_t>(1.0 / m_DeltaTime);
    m_RunTime = m_Timer.Duration();

    //The current frame in flight 
    m_FrameInFlight = m_FrameCount % FRAMES_IN_FLIGHT;
}

void HelloTriangleApp::Synchronize()
{
    EASY_FUNCTION();
    //Wait on a fence for the next frame to be available for processing.
    vkWaitForFences(m_Context.GetDevice(), 1, &m_fFrameReady[m_FrameInFlight], VK_TRUE, UINT64_MAX);
    vkResetFences(m_Context.GetDevice(), 1, &m_fFrameReady[m_FrameInFlight]);

}

void HelloTriangleApp::DrawGUI()
{
    m_ImGuiRenderer.BeginFrame();
    bool bShowDemo = true;
    ImGui::ShowDemoWindow(&bShowDemo);
    m_ImGuiRenderer.EndFrame();

    m_ImGuiRenderer.Draw(&m_Commands[m_FrameInFlight]);
}

void HelloTriangleApp::CreateColourRenderTarget(const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource)
{
    m_Context.CreateImage(VK_IMAGE_TYPE_2D, { extents.width, extents.height, 1 }, MSAASamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VMA_MEMORY_USAGE_AUTO, 0, &pRenderTargetResource->allocation, &pRenderTargetResource->image);

    m_Context.CreateImageView(pRenderTargetResource->image, format, VK_IMAGE_ASPECT_COLOR_BIT, &pRenderTargetResource->view);
}

void HelloTriangleApp::CreateDepthRenderTarget(const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource)
{
    m_Context.CreateImage(VK_IMAGE_TYPE_2D, { extents.width, extents.height, 1 }, MSAASamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VMA_MEMORY_USAGE_AUTO, 0, &pRenderTargetResource->allocation, &pRenderTargetResource->image);

    m_Context.CreateImageView(pRenderTargetResource->image, format, VK_IMAGE_ASPECT_DEPTH_BIT, &pRenderTargetResource->view);
}

void HelloTriangleApp::DestroyImageResource(ImageResource& resource)
{
    m_Context.DestroyImageView(resource.view);
    m_Context.DestroyImage(resource.image, resource.allocation);
}

void HelloTriangleApp::CreateSwapchain(const Window& window, VkSampleCountFlagBits samples)
{
    //TODO: Release old resources if present. 

    m_Swapchain.Create(m_Context, &window, m_QueueFamilyIndex);
    //Create the Render Pass, and its resources
    {
        const VkExtent2D extents = {
            window.GetWidth(),
            window.GetHeight()
        };

        m_RenderTargets.resize(3);
        CreateColourRenderTarget(extents, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, samples, &m_RenderTargets[0]);  //Colour Attachment
        CreateDepthRenderTarget(extents, VkHelpers::FindDepthFormat(m_Context.GetPhysicalDevice()), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, samples, &m_RenderTargets[1]);   //Depth Attachment
        CreateColourRenderTarget(extents, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SAMPLE_COUNT_1_BIT, &m_RenderTargets[2]);   //Resolve Attachment

        //Describe our Image Attachments.
        std::vector<VkAttachmentDescription> attachmentDescs(3);
        {
            attachmentDescs[0] = {
                0,
                VK_FORMAT_B8G8R8A8_UNORM,
                samples,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            attachmentDescs[1] = {
                0,
                VKR::VkHelpers::FindDepthFormat(m_Context.GetPhysicalDevice()),
                samples,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            attachmentDescs[2] = {
                0,
                VK_FORMAT_B8G8R8A8_UNORM,
                VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            };
        }

        //Create References for our Attachments
        std::vector<VkAttachmentReference> attachmentRefs(3);
        {
            //Colour Attachment
            attachmentRefs[0] = {
                0,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            //Depth Attachment
            attachmentRefs[1] = {
                1,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            //Resolve Attachment
            attachmentRefs[2] = {
                2,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };
        }

        //Describe the Subpass, and its dependencies
        VkSubpassDescription subpassDesc =  //TODO: VkInit
        {
            0,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            0,
            nullptr,
            1,
            &attachmentRefs[0],
            &attachmentRefs[2],
            &attachmentRefs[1], //TODO: Handle 1xMSAA option
            0,
            nullptr
        };

        VkSubpassDependency subpassDependency = {
            VK_SUBPASS_EXTERNAL,
            0,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0
        };

        //Create the renderpass object. 
        m_Context.CreateRenderPass(attachmentDescs.size(), attachmentDescs.data(), 1, &subpassDesc, 1, &subpassDependency, &m_RenderPass);
    }

    //Create the Framebuffers
    {
        //Retrieve image view references for our colour attachments
        m_FrameBuffers.resize(m_Swapchain.GetImageCount());
        for (int i = 0; i < m_Swapchain.GetImageCount(); i++) {
            VkImageView attachments[3] = { m_RenderTargets[0].view, m_RenderTargets[1].view ,m_Swapchain.GetImageViews()[i] };
            m_Context.CreateFrameBuffer({ WINDOW_WIDTH, WINDOW_HEIGHT, 1 }, m_RenderPass, 3, attachments, &m_FrameBuffers[i]);
        }
    }

    //Hook in ImGui
    m_ImGuiRenderer.Hook(m_Context, m_QueueFamilyIndex, m_Queue, VK_NULL_HANDLE, m_Swapchain.GetImageCount(), m_RenderPass, samples);
}


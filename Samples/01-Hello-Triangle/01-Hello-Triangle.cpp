/**
*   @file 01-Hello-Triangle.cpp
*   @brief Demo Project utilizing VKR to display a Triangle within a window.
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/28
*/

#include "01-Hello-Triangle.h"
#include <VKR/Vulkan/VkHelpers.h>
#include <VKR/Vulkan/VkPipelineBuilder.h>
#include <VKR/Vulkan/VkInit.h>
#include <VKR/File.h>
#include <VKR/Maths.h>
#include <VKR/Logger.h>
#include <easy/profiler.h>


constexpr uint32_t FRAMES_IN_FLIGHT = 3;
constexpr char* PIPELINE_CACHE_PATH = ".cache";
constexpr VkSampleCountFlagBits SAMPLE_COUNT = VK_SAMPLE_COUNT_4_BIT; 

using namespace VKR;

Samples::HelloTriangleApp::HelloTriangleApp() : DemoApp()
{
    EASY_FUNCTION();

    m_MSAASamples = SAMPLE_COUNT;
    m_CommandPool = VK_NULL_HANDLE;
    m_FrameInFlight = 0;
    m_ImageIndex = 0;
    m_OcclusionQueryPool = VK_NULL_HANDLE;
    m_Pipeline = VK_NULL_HANDLE;
    m_PipelineLayout = VK_NULL_HANDLE;
    m_PipelineQueryPool = VK_NULL_HANDLE;
    m_Queue = VK_NULL_HANDLE;
    m_QueueFamilyIndex = -1;
    m_RenderPass = VK_NULL_HANDLE;
    m_Scissor = {};
    m_Viewport = {};
    m_PipelineStatistics = {}; 
    m_OcclusionStatistics = 0; 
    m_DeviceProperties = {};
}


void Samples::HelloTriangleApp::Init(const VKR::Window& window)
{
    EASY_FUNCTION();
    Log::Message("Initializing Application...\n");
    VKR::Timer initTimer;
    initTimer.Start();

    //Vulkan Instance Creation
    Log::Message("Creating Vulkan Instance.\n");
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
            for (uint32_t i = 0; i < optExtCount; i++) {
                instanceExtensions.push_back(ext[i]);
            }
        }

        VK_CHECK(m_Context.CreateInstance(instanceLayers.size(), instanceLayers.data(), instanceExtensions.size(), instanceExtensions.data(), nullptr));
    }

#if DEBUG
    Log::Message("Creating Vulkan Debug Objects.\n");
    //Vulkan Debug Callback Object Creation
    {
        m_Context.CreateDebugLogger();
        m_Context.CreateDebugReporter();
    }
#endif

    Log::Message("Creating Vulkan Device.\n");
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

        VK_CHECK(m_Context.SelectPhysicalDevice());

        //Retrieve physical device properties
        vkGetPhysicalDeviceProperties(m_Context.GetPhysicalDevice(), &m_DeviceProperties); 

        m_QueueFamilyIndex = VkHelpers::FindQueueFamilyIndex(m_Context.GetPhysicalDevice(), VK_QUEUE_GRAPHICS_BIT);
        float queuePriorities[] = { 1.0f };
        const VkDeviceQueueCreateInfo qci = VkInit::MakeDeviceQueueCreateInfo(m_QueueFamilyIndex, 1, queuePriorities);

        VkPhysicalDeviceFeatures features = {};
        features.pipelineStatisticsQuery = true;
        m_Context.CreateDevice(deviceExtensions.size(), deviceExtensions.data(), 1, &qci, &features);
    }

    m_Context.CreateAllocator();

    m_Queue = m_Context.GetDeviceQueue(m_QueueFamilyIndex, 0);


    Log::Message("Creating Vulkan Resources.\n");
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

    //Initialize the ImGui renderer
    m_ImGuiRenderer.Init(m_Context, window);
    ImGuiIO& io = ImGui::GetIO();

    //Create the Swapchain
    CreateSwapchain(window, m_MSAASamples);
    io.Fonts->AddFontFromFileTTF("Data/Fonts/NotoSansJP-Regular.ttf", 18, NULL, io.Fonts->GetGlyphRangesJapanese());

    Log::Message("Creating Graphics Pipeline.\n");
    //Load the Pipeline Cache if present
    VkPipelineCache pipelineCache;
    {
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
        if (VKR::IO::FileExists(PIPELINE_CACHE_PATH)) {
            Log::Message("Loading Pipeline Cache.\n"); 
            std::vector<char> pipelineCacheBlob;
            VKR::IO::ReadFile(PIPELINE_CACHE_PATH, pipelineCacheBlob);
            pipelineCacheCreateInfo.initialDataSize = pipelineCacheBlob.size();
            pipelineCacheCreateInfo.pInitialData = pipelineCacheBlob.data();
        }

        vkCreatePipelineCache(m_Context.GetDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache);
    }

    //Create the Graphics Pipeline
    {
        m_Context.CreatePipelineLayout(0, nullptr, 0, nullptr, &m_PipelineLayout);
        VkShaderModule vertexShaderModule;
        {
            std::vector<char> vs_source;
            VKR::IO::ReadFile("Data/Shaders/vs_triangle.spirv", vs_source);
            m_Context.CreateShaderModule(vs_source.data(), vs_source.size(), &vertexShaderModule);
        }

        VkShaderModule fragmentShaderModule;
        {
            std::vector<char> fs_source;
            VKR::IO::ReadFile("Data/Shaders/fs_triangle.spirv", fs_source);
            m_Context.CreateShaderModule(fs_source.data(), fs_source.size(), &fragmentShaderModule);
        }

        VkPipelineColorBlendAttachmentState blendAttachment = {};
        blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachment.blendEnable = VK_TRUE;
        blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VKR::VkPipelineBuilder builder;
        builder.AddShaderStage(vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT, "main");
        builder.AddShaderStage(fragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
        builder.SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        builder.SetRasterizerState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        builder.SetMSAAState(m_MSAASamples, false, false);  //TODO: Other MSAA Pipelines
        builder.SetDepthStencilState(true, true, true);
        builder.SetBlendState(false, VK_LOGIC_OP_COPY, 1, &blendAttachment);

        const std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        builder.SetDynamicState(dynamicStates.size(), dynamicStates.data());

        builder.SetViewportState(1, &m_Viewport, 1, &m_Scissor);

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = builder.BuildGraphicsPipeline(m_PipelineLayout, m_RenderPass, 0);

        m_Context.CreateGraphicsPipelines(1, &graphicsPipelineCreateInfo, pipelineCache, &m_Pipeline);

        m_Context.DestroyShaderModule(vertexShaderModule);
        m_Context.DestroyShaderModule(fragmentShaderModule);

        //Write the Pipeline Cache to disk 
        if(!VKR::IO::FileExists(PIPELINE_CACHE_PATH))
        {
            Log::Message("Saving Pipeline Cache.\n"); 
            uint64_t cacheDataSize = 0;
            vkGetPipelineCacheData(m_Context.GetDevice(), pipelineCache, &cacheDataSize, nullptr);
            std::vector<char> cacheData(cacheDataSize);
            vkGetPipelineCacheData(m_Context.GetDevice(), pipelineCache, &cacheDataSize, cacheData.data());

            VKR::IO::WriteFile(PIPELINE_CACHE_PATH, cacheData.data(), cacheData.size());
        }
        vkDestroyPipelineCache(m_Context.GetDevice(), pipelineCache, nullptr);
    }

    Log::Message("Creating Query Pools.\n");
    //Create Query Pools
    {
        m_Context.CreateQueryPool(VK_QUERY_TYPE_PIPELINE_STATISTICS, 1,
            VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT
            , &m_PipelineQueryPool);

        m_Context.CreateQueryPool(VK_QUERY_TYPE_OCCLUSION, 1, 0, &m_OcclusionQueryPool);

    }
    //Start the application timer after resource initialization
    m_Timer.Start();

    initTimer.Tick();
    Log::Message("Application Initialization complete in %fms!\n", initTimer.Duration());
}

void Samples::HelloTriangleApp::Update()
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
            {0, 0, 600, 400},   //TODO: Hook in Swapchain Extents
            3,
            clearValues
        };
        vkCmdBeginRenderPass(m_Commands[m_FrameInFlight], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    //Draw Stuff. 
    vkCmdSetViewport(m_Commands[m_FrameInFlight], 0, 1, &m_Viewport);
    vkCmdSetScissor(m_Commands[m_FrameInFlight], 0, 1, &m_Scissor);
    vkCmdBindPipeline(m_Commands[m_FrameInFlight], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
    vkCmdDraw(m_Commands[m_FrameInFlight], 3, 1, 0, 0);

    DrawGUI();

    vkCmdEndRenderPass(m_Commands[m_FrameInFlight]);
}

void Samples::HelloTriangleApp::FixedUpdate()
{
    EASY_FUNCTION();
}

void Samples::HelloTriangleApp::BeginFrame()
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

    vkCmdResetQueryPool(m_Commands[m_FrameInFlight], m_OcclusionQueryPool, 0, 1);
    vkCmdResetQueryPool(m_Commands[m_FrameInFlight], m_PipelineQueryPool, 0, 1);
    vkCmdBeginQuery(m_Commands[m_FrameInFlight], m_PipelineQueryPool, 0, 0);
    vkCmdBeginQuery(m_Commands[m_FrameInFlight], m_OcclusionQueryPool, 0, 0);
}

void Samples::HelloTriangleApp::EndFrame()
{
    EASY_FUNCTION();

    //Query the pipeline
    vkCmdEndQuery(m_Commands[m_FrameInFlight], m_OcclusionQueryPool, 0);
    vkCmdEndQuery(m_Commands[m_FrameInFlight], m_PipelineQueryPool, 0);

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

    //Retrieve Pipeline Query Results
    {
        const uint64_t bufferSize = static_cast<uint32_t>(sizeof(m_PipelineStatistics));

        vkGetQueryPoolResults(
            m_Context.GetDevice(),
            m_PipelineQueryPool,
            0,
            1,
            bufferSize,
            &m_PipelineStatistics,
            0,
            VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT
        );

        vkGetQueryPoolResults(
            m_Context.GetDevice(),
            m_OcclusionQueryPool,
            0,
            1,
            sizeof(uint64_t),
            &m_OcclusionStatistics,
            0,
            VK_QUERY_CONTROL_PRECISE_BIT
        );
    }

    //Present the image to the screen. 
    m_Swapchain.Present(m_Queue, m_sRenderFinished[m_FrameInFlight], &m_ImageIndex);

    m_FrameCount++; //Update our internal frame counter. 
}

void Samples::HelloTriangleApp::Shutdown()
{
    EASY_FUNCTION();

    //Wait for all device work to be completed before attempting to destroy any objects. 
    vkDeviceWaitIdle(m_Context.GetDevice());

    m_Context.DestroyQueryPool(m_OcclusionQueryPool);
    m_Context.DestroyQueryPool(m_PipelineQueryPool);

    m_ImGuiRenderer.Shutdown(m_Context);

    //Destroy application resources

    m_Context.DestroyPipelineLayout(m_PipelineLayout);
    m_Context.DestroyPipeline(m_Pipeline);

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

void Samples::HelloTriangleApp::UpdateFrameCounter()
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

void Samples::HelloTriangleApp::Synchronize()
{
    EASY_FUNCTION();
    //Wait on a fence for the next frame to be available for processing.
    vkWaitForFences(m_Context.GetDevice(), 1, &m_fFrameReady[m_FrameInFlight], VK_TRUE, UINT64_MAX);
    vkResetFences(m_Context.GetDevice(), 1, &m_fFrameReady[m_FrameInFlight]);

}

void Samples::HelloTriangleApp::DrawGUI()
{
    EASY_FUNCTION();
    m_ImGuiRenderer.BeginFrame();

    //GUI control flags
    static bool bShowDemo = false;
    static bool bShowApplicationStats = false;
    static bool bShowHardwareInfo = false;
    static bool bShowVulkanStats = false;

    if (bShowDemo) {
        ImGui::ShowDemoWindow(&bShowDemo);
    }

    //Render an overlay with application information. 
    ImGuiIO& io = ImGui::GetIO();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    const float padding = 10.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; 
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = work_pos.x + work_size.x - padding;
    window_pos.y = work_pos.y + padding;
    window_pos_pivot.x = 1.0f;
    window_pos_pivot.y = 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    flags |= ImGuiWindowFlags_NoMove;


    if (ImGui::Begin("Application Info", nullptr, flags))
    {
        ImGui::Text("VKR Samples 01 - Hello Triangle\nRight Click here for more options.\nEwan Burnett (2024)");
        if (bShowApplicationStats) {
            ImGui::Separator();
            ImGui::Text("Frame Count: %d", m_FrameCount);
            ImGui::Text("DeltaTime (ms): %f", m_DeltaTime);
            ImGui::Text("FPS: %d", m_FPS);
            ImGui::Text("Runtime (s): %f", m_RunTime);
        }
        if (bShowHardwareInfo) {
            ImGui::Separator(); 
            ImGui::Text("%s", m_DeviceProperties.deviceName);
            char* deviceType = "Other"; 
            switch (m_DeviceProperties.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: 
                deviceType = "Integrated GPU";
                break; 
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: 
                deviceType = "Discrete GPU"; 
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: 
                deviceType = "Virtual GPU"; 
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU: 
                deviceType = "CPU"; 
                break; 
            default: 
                break; 
            }
            ImGui::Text("%s", deviceType);
        }
        if (bShowVulkanStats) {
            ImGui::Separator();
            ImGui::Text("Vertex Shader Invocations: %d", m_PipelineStatistics.vertexShaderInvocations);
            ImGui::Text("Fragment Shader Invocations: %d", m_PipelineStatistics.fragmentShaderInvocations);
            ImGui::Text("Input Vertices Count: %d", m_PipelineStatistics.inputAssemblyVertices);
            ImGui::Text("Input Primitives Count: %d", m_PipelineStatistics.inputAssemblyPrimitives);
            ImGui::Text("Clipping Invocations: %d", m_PipelineStatistics.clippingInvocations);
            ImGui::Text("Clipping Primitives: %d", m_PipelineStatistics.clippingOutputPrimitives);
            ImGui::Text("Passed Fragments: %d", m_OcclusionStatistics);
        }

        if (ImGui::BeginPopupContextWindow())
        {
            ImGui::MenuItem("Show Application Statistics", nullptr, &bShowApplicationStats);
            ImGui::MenuItem("Show Hardware Information", nullptr, &bShowHardwareInfo);
            ImGui::MenuItem("Show Vulkan Statistics", nullptr, &bShowVulkanStats);
            ImGui::MenuItem("Show ImGui Demo", nullptr, &bShowDemo);

            ImGui::EndPopup();
        }

    }
    ImGui::End();

    m_ImGuiRenderer.EndFrame();

    m_ImGuiRenderer.Draw(&m_Commands[m_FrameInFlight]);
}

void Samples::HelloTriangleApp::CreateColourRenderTarget(const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource)
{
    EASY_FUNCTION();
    m_Context.CreateImage(VK_IMAGE_TYPE_2D, { extents.width, extents.height, 1 }, MSAASamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VMA_MEMORY_USAGE_AUTO, 0, &pRenderTargetResource->allocation, &pRenderTargetResource->image);

    m_Context.CreateImageView(pRenderTargetResource->image, format, VK_IMAGE_ASPECT_COLOR_BIT, &pRenderTargetResource->view);
}

void Samples::HelloTriangleApp::CreateDepthRenderTarget(const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource)
{
    EASY_FUNCTION();
    m_Context.CreateImage(VK_IMAGE_TYPE_2D, { extents.width, extents.height, 1 }, MSAASamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VMA_MEMORY_USAGE_AUTO, 0, &pRenderTargetResource->allocation, &pRenderTargetResource->image);

    m_Context.CreateImageView(pRenderTargetResource->image, format, VK_IMAGE_ASPECT_DEPTH_BIT, &pRenderTargetResource->view);
}

void Samples::HelloTriangleApp::DestroyImageResource(ImageResource& resource)
{
    EASY_FUNCTION();
    m_Context.DestroyImageView(resource.view);
    m_Context.DestroyImage(resource.image, resource.allocation);
}

void Samples::HelloTriangleApp::CreateSwapchain(const Window& window, VkSampleCountFlagBits samples)
{
    EASY_FUNCTION();
    Log::Message("Creating Swapchain.\n");
    //TODO: Release old resources if present. 

    const VkExtent2D extents = {
        window.GetWidth(),
        window.GetHeight()
    };

    m_Swapchain.Create(m_Context, &window, m_QueueFamilyIndex);
    //Create the Render Pass, and its resources
    {
        m_Viewport.x = 0;
        m_Viewport.y = 0; // -static_cast<float>(extents.height);
        m_Viewport.width = (float)extents.width;
        m_Viewport.height = (float)extents.height;
        m_Viewport.minDepth = 0.0f;
        m_Viewport.maxDepth = 1.0f;

        m_Scissor.offset = { 0, 0 };
        m_Scissor.extent = extents;

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
                VkHelpers::FindDepthFormat(m_Context.GetPhysicalDevice()),
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
        for (uint32_t i = 0; i < m_Swapchain.GetImageCount(); i++) {
            VkImageView attachments[3] = { m_RenderTargets[0].view, m_RenderTargets[1].view ,m_Swapchain.GetImageViews()[i] };
            m_Context.CreateFrameBuffer({ extents.width, extents.height, 1 }, m_RenderPass, 3, attachments, &m_FrameBuffers[i]);
        }
    }

    //Hook in ImGui
    m_ImGuiRenderer.Hook(m_Context, m_QueueFamilyIndex, m_Queue, VK_NULL_HANDLE, m_Swapchain.GetImageCount(), m_RenderPass, samples);
}


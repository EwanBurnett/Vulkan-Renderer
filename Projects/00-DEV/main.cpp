#include <VKR.h>
#include <Window.h>
#include <Timer.h>
#include <Maths.h>

#include <Vulkan/VkContext.h>
#include <Vulkan/VkHelpers.h>
#include <Vulkan/VkSwapchain.h>
#include <Vulkan/VkInit.h>
#include <Vulkan/VkPipelineBuilder.h>

#include <vector> 
#include <Thread>

#include <fstream>  //DEBUGGING

#include <cstdio> 
#include <easy/profiler.h>

constexpr uint32_t WINDOW_WIDTH = 600;
constexpr uint32_t WINDOW_HEIGHT = 400;

constexpr uint32_t FRAMES_IN_FLIGHT = 3;

void InitVulkan(VKR::VkContext& context, VKR::VkSwapchain& swapchain, VKR::Window& window, uint32_t& queueFamilyIndex, VkQueue& queue);
void InitResources();
void ShutdownVulkan(VKR::VkContext& context, VKR::VkSwapchain& swapchain);

int main() {

    VKR::Init();
    EASY_BLOCK("App Initialization");

    VKR::VkContext context;
    VKR::VkSwapchain swapchain;
    VkQueue graphicsQueue;  //TODO: Queue Wrapper
    uint32_t graphicsQueueIndex;

    VKR::Window window;
    window.Create("Vulkan Renderer", WINDOW_WIDTH, WINDOW_HEIGHT);
    window.Show();

    InitVulkan(context, swapchain, window, graphicsQueueIndex, graphicsQueue);

    VkSemaphore s_ImageAvailable[FRAMES_IN_FLIGHT];
    VkSemaphore s_FrameFinished[FRAMES_IN_FLIGHT];
    VkFence f_FrameReady[FRAMES_IN_FLIGHT];

    for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
        context.CreateSemaphore(&s_ImageAvailable[i]);
        context.CreateSemaphore(&s_FrameFinished[i]);
        context.CreateFence(&f_FrameReady[i]);
    }

    VkCommandPool commandPool;
    context.CreateCommandPool(graphicsQueueIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &commandPool);

    //Testing VMA
    VkBuffer dbgBuffer;
    VmaAllocation dbgBufferAlloc;
    context.CreateBuffer(1024 * 1024 * 256, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, &dbgBufferAlloc, &dbgBuffer);

    VkImage dbgImage;
    VmaAllocation dbgImageAlloc;
    context.CreateImage(VK_IMAGE_TYPE_2D, { 1600, 900, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VMA_MEMORY_USAGE_AUTO, 0, &dbgImageAlloc, &dbgImage);

    //Pipeline Creation
    VkPipeline computePipeline;
    VkPipelineLayout computePipelineLayout;
    context.CreatePipelineLayout(0, nullptr, &computePipelineLayout);

    VkShaderModule computeShaderModule;
    {   //DEBUGGING TEMP
        auto readShaderBlob = [](const char* fileName)  //TODO: File I/O Helpers
        {
            std::ifstream file(fileName, std::ios::ate | std::ios::binary);

            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file!\n");
            }

            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();
            return buffer;
        };


        auto cs_source = readShaderBlob("Shaders/cs.spirv");
        context.CreateShaderModule(cs_source.data(), cs_source.size(), &computeShaderModule);
    }

    VKR::VkPipelineBuilder builder;
    builder.AddShaderStage(computeShaderModule, VK_SHADER_STAGE_COMPUTE_BIT, "main");
    builder.BuildComputePipeline(context, computePipelineLayout, &computePipeline);

    VKR::Timer timer;
    timer.Start();

    uint64_t frameIdx = 0;  //Keep track of the current frame. 
    double runtime = 0;
    uint64_t fps = 0;
    double dtms = 0.0;

    EASY_END_BLOCK;

    while (window.PollEvents()) {
        EASY_BLOCK("Main Loop", profiler::colors::SkyBlue);
        {
            EASY_BLOCK("Timing");
            timer.Tick();
            dtms = timer.DeltaTime();
            fps = 1.0 / dtms;
            runtime = timer.Duration();
        }

        uint32_t imageIdx;
        {
            EASY_BLOCK("Synchronization", profiler::colors::Red500);
            vkWaitForFences(context.GetDevice(), 1, &f_FrameReady[frameIdx % FRAMES_IN_FLIGHT], VK_TRUE, UINT64_MAX);
            vkResetFences(context.GetDevice(), 1, &f_FrameReady[frameIdx % FRAMES_IN_FLIGHT]);
            {
                EASY_BLOCK("Image Acquisition", profiler::colors::Red500);
                vkAcquireNextImageKHR(context.GetDevice(), swapchain.GetSwapchain(), UINT64_MAX, s_ImageAvailable[frameIdx % FRAMES_IN_FLIGHT], nullptr, &imageIdx);
            }
        }

        {
            EASY_BLOCK("Update", profiler::colors::Amber400);
            //Simulate computing WorldViewProjection matrices for an arbitrary number of objects. 
            const VKR::Math::Matrix4x4<> p = VKR::Math::Matrix4x4<>::ProjectionFoVDegrees(90.0, 16.0 / 9.0, 0.001, 100000.0);
            VKR::Math::Matrix4x4<> v = VKR::Math::Matrix4x4<>::View({ 10.0, 32, -34 }, { 1, 2, 3 }, { 1, 2, 0 }, { 1, 0, 0 });  //TODO: Const Operators

            for (int i = 0; i < 500; i++) {
                VKR::Math::Matrix4x4<> s = VKR::Math::Matrix4x4<>::Scaling({ (float)i, 2.0, 1.0 });

                VKR::Math::Matrix4x4<> x = VKR::Math::Matrix4x4<>::XRotationFromDegrees(i * 30 + (10 * dtms));
                VKR::Math::Matrix4x4<> y = VKR::Math::Matrix4x4<>::YRotationFromDegrees(i * 30 + (20 * dtms));
                VKR::Math::Matrix4x4<> z = VKR::Math::Matrix4x4<>::ZRotationFromDegrees(i * 30 + (30 * dtms));

                VKR::Math::Matrix4x4<> t = VKR::Math::Matrix4x4<>::Translation({ (float)frameIdx + (10.0f * (float)dtms), (float)i * 20, 30 }); //TODO: Template Argument Static Typecasting

                VKR::Math::Matrix4x4<> r = (x * (y * z));
                VKR::Math::Matrix4x4<> w = (s * r) * t;
                VKR::Math::Matrix4x4<> wvp = w * (v * p);
            }
        }
        {
            EASY_BLOCK("Log Output");
            printf("\r                                                          "); //Clear the current line for consistent output
            printf("\rFrame %d\tdtms: %04fms\t%d fps\truntime: %fs", frameIdx++, dtms, fps, runtime);
        }

        {
            EASY_BLOCK("Device Work", profiler::colors::Red500);
            VkCommandBuffer cmd;
            const VkCommandBufferAllocateInfo allocInfo = {
                   VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                   nullptr,
                   commandPool,
                   VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                   1
            };

            vkAllocateCommandBuffers(context.GetDevice(), &allocInfo, &cmd);

            const VkCommandBufferBeginInfo beginInfo = {
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                nullptr,
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                nullptr
            };
            {
                EASY_BLOCK("Compute Pass", profiler::colors::Red500);
                vkBeginCommandBuffer(cmd, &beginInfo);
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
                vkCmdDispatch(cmd, 600, 400, 1);
                vkEndCommandBuffer(cmd);
            }

            {
                EASY_BLOCK("Queue Submission", profiler::colors::Red500);

                VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

                const VkSubmitInfo submitInfo = {
                    VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    nullptr,
                    1,
                    &s_ImageAvailable[frameIdx % FRAMES_IN_FLIGHT],
                    waitStages,
                    1,
                    &cmd,
                    1,
                    &s_FrameFinished[frameIdx % FRAMES_IN_FLIGHT]
                };

                vkQueueSubmit(graphicsQueue, 1, &submitInfo, f_FrameReady[frameIdx % FRAMES_IN_FLIGHT]);

                vkFreeCommandBuffers(context.GetDevice(), commandPool, 1, &cmd);
            }

            swapchain.Present(graphicsQueue, s_FrameFinished[frameIdx % FRAMES_IN_FLIGHT], &imageIdx);
        }
    }

    EASY_BLOCK("App Shutdown");

    printf("\n");   //Print a newline for correct logging
    window.Hide();
    window.Destroy();

    vkDeviceWaitIdle(context.GetDevice());
    context.DestroyPipeline(computePipeline);
    context.DestroyPipelineLayout(computePipelineLayout);
    context.DestroyShaderModule(computeShaderModule);

    context.DestroyImage(dbgImage, dbgImageAlloc);
    context.DestroyBuffer(dbgBuffer, dbgBufferAlloc);
    context.DestroyCommandPool(commandPool);

    for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
        context.DestroyFence(f_FrameReady[i]);
        context.DestroySemaphore(s_FrameFinished[i]);
        context.DestroySemaphore(s_ImageAvailable[i]);
    }

    ShutdownVulkan(context, swapchain);

    EASY_END_BLOCK;

    VKR::Shutdown();

    return 0;
}

//--------------------------

void InitVulkan(VKR::VkContext& context, VKR::VkSwapchain& swapchain, VKR::Window& window, uint32_t& queueFamilyIndex, VkQueue& queue) {

    std::vector<const char*> instanceLayers = {
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


    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
#if DEBUG
        VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
#endif
    };


    VK_CHECK(context.CreateInstance(instanceLayers.size(), instanceLayers.data(), instanceExtensions.size(), instanceExtensions.data(), nullptr));
#if DEBUG
    VK_CHECK(context.CreateDebugLogger());
    VK_CHECK(context.CreateDebugReporter());
#endif

    VK_CHECK(context.SelectPhysicalDevice());

    queueFamilyIndex = VKR::VkHelpers::FindQueueFamilyIndex(context.GetPhysicalDevice(), VK_QUEUE_COMPUTE_BIT);
    float queuePriorities[] = { 1.0f };
    const VkDeviceQueueCreateInfo qci = VKR::VkInit::MakeDeviceQueueCreateInfo(0, 1, queuePriorities);

    VkPhysicalDeviceFeatures features = {};
    features.geometryShader = VK_TRUE;
    context.CreateDevice(deviceExtensions.size(), deviceExtensions.data(), 1, &qci, &features);

    context.CreateAllocator();

    queue = context.GetDeviceQueue(queueFamilyIndex, 0);

    swapchain.Create(context, &window, queueFamilyIndex);
}


//For Application Bootstrapping
void InitResources() {

}

void ShutdownVulkan(VKR::VkContext& context, VKR::VkSwapchain& swapchain) {

    vkDeviceWaitIdle(context.GetDevice());
    swapchain.Destroy(context);
    context.DestroyAllocator();
    context.DestroyDevice();

#ifdef DEBUG
    context.DestroyDebugReporter();
    context.DestroyDebugLogger();
#endif

    context.DestroyInstance();
}


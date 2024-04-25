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
    context.CreateCommandPool(graphicsQueueIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, &commandPool);

    //Testing VMA
    VkBuffer dbgBuffer;
    VmaAllocation dbgBufferAlloc;
    context.CreateBuffer(sizeof(float) * 8 * 3, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, &dbgBufferAlloc, &dbgBuffer);

    VkImage depthImage;
    VmaAllocation depthImageAlloc;
    VkImageView depthView;

    const VkFormat depthFormat = VKR::VkHelpers::FindDepthFormat(context.GetPhysicalDevice());
    context.CreateImage(VK_IMAGE_TYPE_2D, { WINDOW_WIDTH, WINDOW_HEIGHT, 1 }, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, &depthImageAlloc, &depthImage);
    context.CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &depthView);
    //TODO: Image Layout Transitions!

    //Pipeline Creation
    VkPipeline computePipeline;
    VkPipelineLayout computePipelineLayout;
    context.CreatePipelineLayout(0, nullptr, &computePipelineLayout);

    VkPipeline graphicsPipeline;
    VkPipelineLayout graphicsPipelineLayout;
    context.CreatePipelineLayout(0, nullptr, &graphicsPipelineLayout);

    VkRenderPass renderPass;
    {
        {
            VkAttachmentDescription attachments[2];
            attachments[0] = {
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

            attachments[1] = {
                0,
                VKR::VkHelpers::FindDepthFormat(context.GetPhysicalDevice()),
                VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            VkSubpassDependency dependencies[2];
            dependencies[0] = {
                VK_SUBPASS_EXTERNAL,
                0,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                0
            };

            dependencies[1] = {
                VK_SUBPASS_EXTERNAL,
                0,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_DEPENDENCY_BY_REGION_BIT
            };

            VkAttachmentReference colorAttachmentRef = {
                0,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            VkAttachmentReference depthAttachmentRef = {
                1,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            VkSubpassDescription subpass = {
                0,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                0,
                nullptr,
                1,
                &colorAttachmentRef,
                nullptr,
                &depthAttachmentRef,
                0,
                nullptr
            };

            context.CreateRenderPass(2, attachments, 1, &subpass, 1, dependencies, &renderPass);
        }
    }

    VkFramebuffer frameBuffers[FRAMES_IN_FLIGHT];
    for (int i = 0; i < 3; i++) {   //TODO: Dispatch through VkContext
        VkImageView attachments[2] = { swapchain.GetImageViews()[i], depthView };
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = attachments;
        createInfo.width = WINDOW_WIDTH;
        createInfo.height = WINDOW_HEIGHT;
        createInfo.layers = 1;

        vkCreateFramebuffer(context.GetDevice(), &createInfo, nullptr, &frameBuffers[i]);
    }


    VkShaderModule computeShaderModule;
    VkShaderModule vertexShaderModule;
    VkShaderModule fragmentShaderModule;
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

        auto vs_source = readShaderBlob("Shaders/vs.spirv");
        context.CreateShaderModule(vs_source.data(), vs_source.size(), &vertexShaderModule);

        auto fs_source = readShaderBlob("Shaders/fs.spirv");
        context.CreateShaderModule(fs_source.data(), fs_source.size(), &fragmentShaderModule);
    }

    VKR::VkPipelineBuilder computeBuilder;
    computeBuilder.AddShaderStage(computeShaderModule, VK_SHADER_STAGE_COMPUTE_BIT, "main");
    computeBuilder.BuildComputePipeline(context, computePipelineLayout, &computePipeline);

    VKR::VkPipelineBuilder builder;
    builder.AddShaderStage(vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT, "main");
    builder.AddShaderStage(fragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT, "main");

    VkVertexInputBindingDescription bindings[1] = {};
    bindings[0].binding = 0;
    bindings[0].stride = sizeof(float) * 8;
    bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributes[3] = {};
    attributes[0].location = 0;
    attributes[0].binding = bindings[0].binding;
    attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[0].offset = 0;

    attributes[1].location = 1;
    attributes[1].binding = bindings[0].binding;
    attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributes[1].offset = sizeof(float) * 3;

    attributes[2].location = 2;
    attributes[2].binding = bindings[0].binding;
    attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[2].offset = sizeof(float) * 5;

    builder.SetVertexInputState(1, bindings, 3, attributes);
    builder.SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)WINDOW_WIDTH;
    viewport.height = (float)WINDOW_HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { 600, 400 };
    builder.SetViewportState(1, &viewport, 1, &scissor);
    builder.SetRasterizerState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
    builder.SetMSAAState(VK_SAMPLE_COUNT_1_BIT, false, false);
    builder.SetDepthStencilState();

    VkPipelineColorBlendAttachmentState blendAttachment = {};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachment.blendEnable = VK_FALSE;
    blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    builder.SetBlendState(false, VK_LOGIC_OP_COPY, 1, &blendAttachment);

    builder.BuildGraphicsPipeline(context, graphicsPipelineLayout, renderPass, 0, &graphicsPipeline);

    std::vector<VkCommandBuffer> commands(FRAMES_IN_FLIGHT);

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
        const uint64_t frame_in_flight = frameIdx % FRAMES_IN_FLIGHT;

        uint32_t imageIdx;
        {
            EASY_BLOCK("Synchronization", profiler::colors::Red500);
            vkWaitForFences(context.GetDevice(), 1, &f_FrameReady[frame_in_flight], VK_TRUE, UINT64_MAX);
            vkResetFences(context.GetDevice(), 1, &f_FrameReady[frame_in_flight]);
            {
                EASY_BLOCK("Image Acquisition", profiler::colors::Red500);
                vkAcquireNextImageKHR(context.GetDevice(), swapchain.GetSwapchain(), UINT64_MAX, s_ImageAvailable[frame_in_flight], nullptr, &imageIdx);
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
            printf("\rFrame %d\tdtms: %04fms\t%d fps\truntime: %fs", frameIdx, dtms, fps, runtime);
        }

        {
            EASY_BLOCK("Device Work", profiler::colors::Red500);


            const VkCommandBufferAllocateInfo allocInfo = {
                   VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                   nullptr,
                   commandPool,
                   VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                   1
            };

            vkFreeCommandBuffers(context.GetDevice(), commandPool, 1, &commands[frame_in_flight]);
            vkAllocateCommandBuffers(context.GetDevice(), &allocInfo, &commands[frame_in_flight]);
            VkCommandBuffer& cmd = commands[frame_in_flight];

            const VkCommandBufferBeginInfo beginInfo = {
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                nullptr,
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                nullptr
            };
            vkBeginCommandBuffer(cmd, &beginInfo);
            {
                EASY_BLOCK("Compute Pass", profiler::colors::Red500);
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
                vkCmdDispatch(cmd, 600, 400, 1);
            }
            {
                EASY_BLOCK("Render Pass", profiler::colors::Red500);
                VkClearValue clearValues[2] = { swapchain.GetColourClearValue(), swapchain.GetDepthStencilClearValue() };
                VkRenderPassBeginInfo rpb = {
                    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                    nullptr,
                    renderPass,
                    frameBuffers[imageIdx],
                    {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
                    2,
                    clearValues
                };
                vkCmdBeginRenderPass(cmd, &rpb, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
                VkDeviceSize offsets = 0;
                vkCmdBindVertexBuffers(cmd, 0, 1, &dbgBuffer, &offsets);
                vkCmdDraw(cmd, 3, 1, 0, 0);

                vkCmdEndRenderPass(cmd);
            }
            vkEndCommandBuffer(cmd);

            {
                EASY_BLOCK("Compute Queue Submission", profiler::colors::Red500);

                VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

                const VkSubmitInfo submitInfo = {
                    VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    nullptr,
                    1,
                    &s_ImageAvailable[frame_in_flight],
                    waitStages,
                    1,
                    &commands[frame_in_flight],
                    1,
                    &s_FrameFinished[frame_in_flight]
                };

                vkQueueSubmit(graphicsQueue, 1, &submitInfo, f_FrameReady[frame_in_flight]);
            }

            swapchain.Present(graphicsQueue, s_FrameFinished[frame_in_flight], &imageIdx);
            frameIdx++;
        }
    }

    EASY_BLOCK("App Shutdown");

    printf("\n");   //Print a newline for correct logging
    window.Hide();
    window.Destroy();

    vkDeviceWaitIdle(context.GetDevice());
    context.DestroyPipeline(graphicsPipeline);
    context.DestroyPipelineLayout(graphicsPipelineLayout);
    context.DestroyShaderModule(fragmentShaderModule);
    context.DestroyShaderModule(vertexShaderModule);

    context.DestroyPipeline(computePipeline);
    context.DestroyPipelineLayout(computePipelineLayout);
    context.DestroyShaderModule(computeShaderModule);

    context.DestroyRenderPass(renderPass);

    for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
        vkDestroyFramebuffer(context.GetDevice(), frameBuffers[i], nullptr);
    }

    context.DestroyImage(depthImage, depthImageAlloc);
    context.DestroyImageView(depthView);
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

    queueFamilyIndex = VKR::VkHelpers::FindQueueFamilyIndex(context.GetPhysicalDevice(), VK_QUEUE_GRAPHICS_BIT);
    float queuePriorities[] = { 1.0f };
    const VkDeviceQueueCreateInfo qci = VKR::VkInit::MakeDeviceQueueCreateInfo(0, 1, queuePriorities);

    VkPhysicalDeviceFeatures features = {};
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


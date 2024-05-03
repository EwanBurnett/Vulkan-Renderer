#include <VKR/VKR.h>
#include <VKR/Window.h>
#include <VKR/Timer.h>
#include <VKR/Maths.h>
#include <VKR/File.h>
#include <VKR/Vulkan/VkContext.h>
#include <VKR/Vulkan/VkHelpers.h>
#include <VKR/Vulkan/VkSwapchain.h>
#include <VKR/Vulkan/VkInit.h>
#include <VKR/Vulkan/VkImGui.h>
#include <VKR/Vulkan/VkPipelineBuilder.h>

#include <vector> 
#include <Thread>

#include <cstdio> 
#include <easy/profiler.h>

constexpr uint32_t WINDOW_WIDTH = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;

constexpr uint32_t FRAMES_IN_FLIGHT = 3;

constexpr uint32_t OBJECT_COUNT = 100;

constexpr VkSampleCountFlagBits MSAA_SAMPLES = VK_SAMPLE_COUNT_4_BIT;

void InitVulkan(VKR::VkContext& context, VKR::VkSwapchain& swapchain, VKR::Window& window, uint32_t& queueFamilyIndex, VkQueue& queue);
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

    VkDescriptorPool descriptorPool;
    const std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {
        {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            swapchain.GetImageCount() * 1
        }
    };
    context.CreateDescriptorPool(swapchain.GetImageCount(), 0, descriptorPoolSizes.size(), descriptorPoolSizes.data(), &descriptorPool);

    VkDescriptorSetLayout descriptorSetLayout;
    const std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT}
    };
    context.CreateDescriptorSetLayout(descriptorSetLayoutBindings.size(), descriptorSetLayoutBindings.data(), &descriptorSetLayout);

    VkDescriptorSet descriptorSet;
    context.AllocateDescriptorSets(descriptorPool, 1, &descriptorSetLayout, &descriptorSet);

    VkBuffer uniformBuffer;
    VmaAllocation uniformBufferAlloc;
    context.CreateBuffer(sizeof(VKR::Math::Matrix4x4<float>), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, &uniformBufferAlloc, &uniformBuffer);

    {
        VkDescriptorBufferInfo bufferInfo = {
            uniformBuffer,
            0,
            sizeof(VKR::Math::Matrix4x4<float>)
        };

        VkWriteDescriptorSet dsWrite = VKR::VkInit::MakeWriteDescriptorSet(descriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &bufferInfo, nullptr);

        vkUpdateDescriptorSets(context.GetDevice(), 1, &dsWrite, 0, nullptr);
    }

    std::vector<float> vertices = {
        -1.0, -1.0, -1.0, 0.0, 0.0, 0.0,
        1.0, -1.0, -1.0, 0.0, 0.0, 1.0,
        1.0, 1.0, -1.0, 0.0, 1.0, 0.0,
        -1.0, 1.0, -1.0, 0.0, 1.0, 1.0,
        -1.0, -1.0, 1.0, 1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
        -1.0, 1.0, 1.0, 1.0, 1.0, 1.0
    };

    std::vector<uint32_t> indices = {
        0, 3, 1,
        3, 2, 1,
        1, 2, 5,
        2, 6, 5,
        5, 6, 4,
        6, 7, 4,
        4, 7, 0,
        7, 3, 0,
        3, 7, 2,
        7, 6, 2,
        4, 0, 5,
        0, 1, 5,
    };
    /*
    std::vector<float> vertices = {
        -0.6f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, -0.6f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.6f, 0.4f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    */

    VkBuffer vertexBuffer;
    VmaAllocation vertexBufferAlloc;
    context.CreateBuffer(sizeof(float) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, &vertexBufferAlloc, &vertexBuffer);

    VkBuffer indexBuffer;
    VmaAllocation indexBufferAlloc;
    context.CreateBuffer(sizeof(uint32_t) * indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, &indexBufferAlloc, &indexBuffer);


    //Copy our Vertex and Index data into their buffers. 
    {
        void* pData = nullptr;
        context.Map(vertexBufferAlloc, &pData);
        memcpy(pData, vertices.data(), sizeof(float) * vertices.size());
        context.Unmap(vertexBufferAlloc);

        context.Map(indexBufferAlloc, &pData);
        memcpy(pData, indices.data(), sizeof(uint32_t) * indices.size());
        context.Unmap(indexBufferAlloc);
    }
    VkImage renderTargetImage;
    VmaAllocation renderTargetImageAlloc;
    VkImageView renderTargetView;
    context.CreateImage(VK_IMAGE_TYPE_2D, { WINDOW_WIDTH, WINDOW_HEIGHT, 1 }, MSAA_SAMPLES, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, &renderTargetImageAlloc, &renderTargetImage);
    context.CreateImageView(renderTargetImage, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &renderTargetView);

    VkImage depthImage;
    VmaAllocation depthImageAlloc;
    VkImageView depthView;

    const VkFormat depthFormat = VKR::VkHelpers::FindDepthFormat(context.GetPhysicalDevice());
    context.CreateImage(VK_IMAGE_TYPE_2D, { WINDOW_WIDTH, WINDOW_HEIGHT, 1 }, MSAA_SAMPLES, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, &depthImageAlloc, &depthImage);
    context.CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &depthView);

    //Pipeline Creation
    VkPipelineCache pipelineCache;
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };//TODO: VKINIT
    if (VKR::IO::FileExists("PipelineCache.bin")) {
        std::vector<char> pipelineCacheBlob;
        VKR::IO::ReadFile("PipelineCache.bin", pipelineCacheBlob);
        pipelineCacheCreateInfo.initialDataSize = pipelineCacheBlob.size();
        pipelineCacheCreateInfo.pInitialData = pipelineCacheBlob.data();
    }

    vkCreatePipelineCache(context.GetDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache);

    VkPipeline computePipeline;
    VkPipelineLayout computePipelineLayout;
    context.CreatePipelineLayout(0, nullptr, 0, nullptr, &computePipelineLayout);
    VkComputePipelineCreateInfo computePipelineCreateInfo;


    const VkPushConstantRange pushConstants = {
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(VKR::Math::Matrix4x4<float>)
    };

    VkPipeline gridPipeline;

    VkPipeline graphicsPipeline;
    VkPipelineLayout graphicsPipelineLayout;
    context.CreatePipelineLayout(1, &descriptorSetLayout, 1, &pushConstants, &graphicsPipelineLayout);

    VkRenderPass renderPass;
    {
        VkAttachmentDescription attachments[3];
        attachments[0] = {
            0,
            VK_FORMAT_B8G8R8A8_UNORM,
            MSAA_SAMPLES,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        attachments[1] = {
            0,
            VKR::VkHelpers::FindDepthFormat(context.GetPhysicalDevice()),
            MSAA_SAMPLES,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };

        attachments[2] = {
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


        VkSubpassDependency dependencies[1];
        dependencies[0] = {
            VK_SUBPASS_EXTERNAL,
            0,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0
        };

        VkAttachmentReference colorAttachmentRef = {
            0,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkAttachmentReference depthAttachmentRef = {
            1,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };


        VkAttachmentReference colorAttachmentResolveRef = {
            2,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };


        VkSubpassDescription subpass = {
            0,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            0,
            nullptr,
            1,
            &colorAttachmentRef,
            &colorAttachmentResolveRef,
            &depthAttachmentRef,
            0,
            nullptr
        };

        context.CreateRenderPass(3, attachments, 1, &subpass, 1, dependencies, &renderPass);
    }

    VkFramebuffer frameBuffers[8];  //Max Framebuffer count
    for (int i = 0; i < swapchain.GetImageCount(); i++) {
        VkImageView attachments[3] = { renderTargetView, depthView ,swapchain.GetImageViews()[i] };
        context.CreateFrameBuffer({ WINDOW_WIDTH, WINDOW_HEIGHT, 1 }, renderPass, 3, attachments, &frameBuffers[i]);
    }

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)WINDOW_WIDTH;
    viewport.height = (float)WINDOW_HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { WINDOW_WIDTH, WINDOW_HEIGHT };


    VkShaderModule computeShaderModule;
    {
        std::vector<char> cs_source;
        VKR::IO::ReadFile("Shaders/cs.spirv", cs_source);
        context.CreateShaderModule(cs_source.data(), cs_source.size(), &computeShaderModule);
    }

    VKR::VkPipelineBuilder computeBuilder;
    computeBuilder.AddShaderStage(computeShaderModule, VK_SHADER_STAGE_COMPUTE_BIT, "main");
    computePipelineCreateInfo = computeBuilder.BuildComputePipeline(computePipelineLayout);
    context.CreateComputePipelines(1, &computePipelineCreateInfo, pipelineCache, &computePipeline);



    VkShaderModule vertexShaderModule;
    {
        std::vector<char> vs_source;
        VKR::IO::ReadFile("Shaders/vs.spirv", vs_source);
        context.CreateShaderModule(vs_source.data(), vs_source.size(), &vertexShaderModule);
    }

    VkShaderModule fragmentShaderModule;
    {
        std::vector<char> fs_source;
        VKR::IO::ReadFile("Shaders/fs.spirv", fs_source);
        context.CreateShaderModule(fs_source.data(), fs_source.size(), &fragmentShaderModule);
    }


    VKR::VkPipelineBuilder builder;
    builder.AddShaderStage(vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT, "main");
    builder.AddShaderStage(fragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT, "main");

    VkVertexInputBindingDescription bindings[1] = {};
    bindings[0].binding = 0;
    bindings[0].stride = sizeof(float) * 6;
    bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributes[2] = {};
    attributes[0].location = 0;
    attributes[0].binding = bindings[0].binding;
    attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[0].offset = 0;

    attributes[1].location = 1;
    attributes[1].binding = bindings[0].binding;
    attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[1].offset = sizeof(float) * 3;

    builder.SetVertexInputState(1, bindings, 2, attributes);
    builder.SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

    builder.SetTessellationState(0);

    builder.SetViewportState(1, &viewport, 1, &scissor);
    builder.SetRasterizerState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
    builder.SetMSAAState(MSAA_SAMPLES, false, false);
    builder.SetDepthStencilState(true, true, true);

    VkPipelineColorBlendAttachmentState blendAttachment = {};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachment.blendEnable = VK_TRUE;
    blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    builder.SetBlendState(false, VK_LOGIC_OP_COPY, 1, &blendAttachment);

    const std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    builder.SetDynamicState(dynamicStates.size(), dynamicStates.data());

    VkShaderModule gridVertexShaderModule;
    {
        std::vector<char> grid_vs_source;
        VKR::IO::ReadFile("Shaders/vs_grid.spirv", grid_vs_source);
        context.CreateShaderModule(grid_vs_source.data(), grid_vs_source.size(), &gridVertexShaderModule);
    }

    VkShaderModule gridFragmentShaderModule;
    {
        std::vector<char> grid_fs_source;
        VKR::IO::ReadFile("Shaders/fs_grid.spirv", grid_fs_source);
        context.CreateShaderModule(grid_fs_source.data(), grid_fs_source.size(), &gridFragmentShaderModule);
    }

    VKR::VkPipelineBuilder gridBuilder;
    gridBuilder.AddShaderStage(gridVertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT, "main");
    gridBuilder.AddShaderStage(gridFragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
    gridBuilder.SetVertexInputState(0, nullptr, 0, nullptr);
    gridBuilder.SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
    gridBuilder.SetTessellationState(0);
    gridBuilder.SetViewportState(1, &viewport, 1, &scissor);
    gridBuilder.SetRasterizerState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    gridBuilder.SetMSAAState(MSAA_SAMPLES, false, false);
    gridBuilder.SetDepthStencilState(false, true, false);

    gridBuilder.SetBlendState(false, VK_LOGIC_OP_COPY, 1, &blendAttachment);
    gridBuilder.SetDynamicState(dynamicStates.size(), dynamicStates.data());

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = builder.BuildGraphicsPipeline(graphicsPipelineLayout, renderPass, 0);


    context.CreateGraphicsPipelines(1, &graphicsPipelineCreateInfo, pipelineCache, &graphicsPipeline);
    graphicsPipelineCreateInfo = gridBuilder.BuildGraphicsPipeline(graphicsPipelineLayout, renderPass, 0);
    context.CreateGraphicsPipelines(1, &graphicsPipelineCreateInfo, pipelineCache, &gridPipeline);


    VKR::VkImGui imGuiRenderer;
    imGuiRenderer.Init(context, window);
    imGuiRenderer.Hook(context, graphicsQueueIndex, graphicsQueue, pipelineCache, swapchain.GetImageCount(), renderPass, MSAA_SAMPLES);

    std::vector<VkCommandBuffer> commands(FRAMES_IN_FLIGHT);

    VKR::Timer timer;
    timer.Start();

    uint64_t frameIdx = 0;  //Keep track of the current frame. 
    double runtime = 0;
    uint64_t fps = 0;
    double dtms = 0.0;

    VKR::Math::Matrix4x4<float> viewProjection = VKR::Math::Matrix4x4<>::Identity();
    std::vector<VKR::Math::Matrix4x4<float>> worldMatrices(OBJECT_COUNT);
            VKR::Math::Vector3f eyePos;
            VKR::Math::Vector3f eyeDir = VKR::Math::Vector3f::Forwards();
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
        imGuiRenderer.BeginFrame();

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
            if (glfwGetKey(window.GLFWHandle(), GLFW_KEY_W)) {
                eyePos.z += 30.0f * dtms;
            }
            else if (glfwGetKey(window.GLFWHandle(), GLFW_KEY_S)) {
                eyePos.z -= 30.0f * dtms;
            }
            if (glfwGetKey(window.GLFWHandle(), GLFW_KEY_A)) {
                eyePos.x -= 30.0f * dtms;
            }
            else if (glfwGetKey(window.GLFWHandle(), GLFW_KEY_D)) {
                eyePos.x += 30.0f * dtms;
            }
            if (glfwGetKey(window.GLFWHandle(), GLFW_KEY_E)) {
                eyePos.y += 30.0f * dtms;
            }
            else if (glfwGetKey(window.GLFWHandle(), GLFW_KEY_Q)) {
                eyePos.y -= 30.0f * dtms;
            }

            printf("\rPosition: (%f, %f, %f)", eyePos.x, eyePos.y, eyePos.z);

            //Compute View-Projection matrix for this frame. 
            VKR::Math::Matrix4x4<> v = VKR::Math::Matrix4x4<>::View(eyePos, eyeDir);  //TODO: Const Operators
            VKR::Math::Matrix4x4<> p = VKR::Math::Matrix4x4<>::ProjectionFoVDegrees(90.0, (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT, 0.001, 100000.0);

            viewProjection = v * p;

            //Compute World matrices for an arbitrary number of objects. 
            for (int i = 0; i < OBJECT_COUNT; i++) {
                VKR::Math::Matrix4x4<> s = VKR::Math::Matrix4x4<>::Scaling({ 1.0, 1.0, 1.0 });

                static float rot = 0.0f;
                rot += dtms;

                VKR::Math::Matrix4x4<> x = VKR::Math::Matrix4x4<>::XRotationFromDegrees(rot / 2.0 + i);
                VKR::Math::Matrix4x4<> y = VKR::Math::Matrix4x4<>::YRotationFromDegrees(rot + i);
                VKR::Math::Matrix4x4<> z = VKR::Math::Matrix4x4<>::ZRotationFromDegrees(0);

                VKR::Math::Matrix4x4<> t = VKR::Math::Matrix4x4<>::Translation({ sinf(i) * OBJECT_COUNT / 10, (float)(i % 100), cosf(i) * OBJECT_COUNT / 10 }); //TODO: Template Argument Static Typecasting

                VKR::Math::Matrix4x4<> r = (x * (y * z));
                worldMatrices[i] = (s * r) * t;
            }
        }

        {
            EASY_BLOCK("Device Work", profiler::colors::Red500);

            context.FreeCommandBuffers(commandPool, 1, &commands[frame_in_flight]);
            context.AllocateCommandBuffers(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &commands[frame_in_flight]);

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
                vkCmdUpdateBuffer(cmd, uniformBuffer, 0, sizeof(VKR::Math::Matrix4x4<float>), &viewProjection);
                VkClearValue clearValues[3] = { swapchain.GetColourClearValue(), swapchain.GetDepthStencilClearValue(), swapchain.GetColourClearValue() };
                VkRenderPassBeginInfo rpb = {
                    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                    nullptr,
                    renderPass,
                    frameBuffers[imageIdx],
                    {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
                    3,
                    clearValues
                };
                vkCmdBeginRenderPass(cmd, &rpb, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdSetViewport(cmd, 0, 1, &viewport);     //Dynamic State
                vkCmdSetScissor(cmd, 0, 1, &scissor);
                vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
                VkDeviceSize offsets = 0;
                vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offsets);
                vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gridPipeline);
                vkCmdDraw(cmd, 6, 1, 0, 0);
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
                for (uint32_t i = 0; i < OBJECT_COUNT; i++) {
                    vkCmdPushConstants(cmd, graphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VKR::Math::Matrix4x4<>), &worldMatrices[i]);
                    vkCmdDrawIndexed(cmd, 36, 1, 0, 0, 0);
                }
                
                ImGui::Begin("Debug");
                ImGui::Text("Debug Message!");
                ImGui::End();

                bool demo = true; 
                ImGui::ShowDemoWindow(&demo);

                ImGui::Begin("Camera"); 
                ImGui::DragFloat3("Orientation", eyeDir.arr, 0.025f, -1.0f, 1.0f); 
                ImGui::End();

                imGuiRenderer.EndFrame();
                imGuiRenderer.Draw(&cmd);

                vkCmdEndRenderPass(cmd);

            }
            vkEndCommandBuffer(cmd);

            {
                EASY_BLOCK("Queue Submission", profiler::colors::Red500);

                VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

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

    uint64_t cacheDataSize = 0;
    vkGetPipelineCacheData(context.GetDevice(), pipelineCache, &cacheDataSize, nullptr);
    std::vector<char> cacheData(cacheDataSize);
    vkGetPipelineCacheData(context.GetDevice(), pipelineCache, &cacheDataSize, cacheData.data());

    VKR::IO::WriteFile("PipelineCache.bin", cacheData.data(), cacheData.size());
    vkDestroyPipelineCache(context.GetDevice(), pipelineCache, nullptr);

    imGuiRenderer.Shutdown(context);

    context.DestroyPipeline(gridPipeline);
    context.DestroyShaderModule(gridFragmentShaderModule);
    context.DestroyShaderModule(gridVertexShaderModule);

    context.DestroyPipeline(graphicsPipeline);
    context.DestroyPipelineLayout(graphicsPipelineLayout);
    context.DestroyShaderModule(fragmentShaderModule);
    context.DestroyShaderModule(vertexShaderModule);

    context.DestroyPipeline(computePipeline);
    context.DestroyPipelineLayout(computePipelineLayout);
    context.DestroyShaderModule(computeShaderModule);

    context.DestroyRenderPass(renderPass);

    for (int i = 0; i < swapchain.GetImageCount(); i++) {
        vkDestroyFramebuffer(context.GetDevice(), frameBuffers[i], nullptr);
    }

    context.DestroyImageView(renderTargetView);
    context.DestroyImage(renderTargetImage, renderTargetImageAlloc);
    context.DestroyImageView(depthView);
    context.DestroyImage(depthImage, depthImageAlloc);
    context.DestroyBuffer(indexBuffer, indexBufferAlloc);
    context.DestroyBuffer(vertexBuffer, vertexBufferAlloc);
    context.DestroyBuffer(uniformBuffer, uniformBufferAlloc);

    context.DestroyDescriptorSetlayout(descriptorSetLayout);
    context.DestroyDescriptorPool(descriptorPool);

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
#if VKR_DEBUG
       "VK_LAYER_KHRONOS_validation",
#endif
    };

    std::vector<const char*> instanceExtensions = {
#if VKR_DEBUG
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
#if VKR_DEBUG
        VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
#endif
    };


    VK_CHECK(context.CreateInstance(instanceLayers.size(), instanceLayers.data(), instanceExtensions.size(), instanceExtensions.data(), nullptr));
#if VKR_DEBUG
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


void ShutdownVulkan(VKR::VkContext& context, VKR::VkSwapchain& swapchain) {

    vkDeviceWaitIdle(context.GetDevice());
    swapchain.Destroy(context);
    context.DestroyAllocator();
    context.DestroyDevice();

#if VKR_DEBUG
    context.DestroyDebugReporter();
    context.DestroyDebugLogger();
#endif

    context.DestroyInstance();
}


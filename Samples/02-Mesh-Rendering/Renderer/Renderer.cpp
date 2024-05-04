#include "Renderer.h"
#include <easy/profiler.h>
#include <VKR/Logger.h>
#include <VKR/Vulkan/VkHelpers.h>
#include <VKR/Vulkan/VkPipelineBuilder.h>
#include <VKR/File.h>

using namespace VKR;

void Renderer::Init(const VKR::VkContext& context, const VKR::Window& window)
{
    m_MSAASamples = VK_SAMPLE_COUNT_4_BIT;  //TODO: MSAA Settings
    m_QueueFamilyIndex = VkHelpers::FindQueueFamilyIndex(context.GetPhysicalDevice(), VK_QUEUE_GRAPHICS_BIT);
    vkGetDeviceQueue(context.GetDevice(), m_QueueFamilyIndex, 0, &m_Queue);
    CreateSwapchain(context, window);

    m_GUI.Init(context, window);
    m_GUI.Hook(context, m_QueueFamilyIndex, m_Queue, m_Swapchain.GetImageCount(), m_RenderPass, m_MSAASamples);

    context.CreateSemaphore(&m_sImageAvailable); //TODO: Proper Synchronization
    context.CreateSemaphore(&m_sRenderFinished); //TODO: Proper Synchronization
    context.CreateFence(&m_fFrameReady, true); 

    context.CreateCommandPool(m_QueueFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, &m_CommandPool);

    //TODO: TEMP RACE CONDITION FIX
    context.AllocateCommandBuffers(m_CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &m_CommandBuffer);

    //INIT GRID PIPELINE
    {

        const VkPushConstantRange pushConstants = {
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(VKR::Math::Matrix4x4<float>)
        };

        VkPipelineLayout graphicsPipelineLayout;
        context.CreatePipelineLayout(0, nullptr,0, nullptr, &graphicsPipelineLayout);

        VkShaderModule gridVertexShaderModule;
        {
            std::vector<char> grid_vs_source;
            VKR::IO::ReadFile("Data/Shaders/grid.vert.spirv", grid_vs_source);
            context.CreateShaderModule(grid_vs_source.data(), grid_vs_source.size(), &gridVertexShaderModule);
        }

        VkShaderModule gridFragmentShaderModule;
        {
            std::vector<char> grid_fs_source;
            VKR::IO::ReadFile("Data/Shaders/grid.frag.spirv", grid_fs_source);
            context.CreateShaderModule(grid_fs_source.data(), grid_fs_source.size(), &gridFragmentShaderModule);
        }

        VKR::VkPipelineBuilder gridBuilder;
        gridBuilder.AddShaderStage(gridVertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT, "main");
        gridBuilder.AddShaderStage(gridFragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
        gridBuilder.SetVertexInputState(0, nullptr, 0, nullptr);
        gridBuilder.SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        gridBuilder.SetTessellationState(0);
        gridBuilder.SetViewportState(1, &m_Viewport, 1, &m_Scissor);
        gridBuilder.SetRasterizerState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        gridBuilder.SetMSAAState(m_MSAASamples, false, false);
        gridBuilder.SetDepthStencilState(false, true, false);

        //gridBuilder.SetBlendState(false, VK_LOGIC_OP_COPY, 1, &blendAttachment);
        //gridBuilder.SetDynamicState(dynamicStates.size(), dynamicStates.data());

        VkGraphicsPipelineCreateInfo createInfo = gridBuilder.BuildGraphicsPipeline(graphicsPipelineLayout, m_RenderPass, 0);


        context.CreateGraphicsPipelines(1, &createInfo, VK_NULL_HANDLE, &m_GridPipeline);
    }
}

void Renderer::Shutdown(const VKR::VkContext& context)
{
    vkDeviceWaitIdle(context.GetDevice()); 

    context.DestroyFence(m_fFrameReady);
    context.DestroySemaphore(m_sImageAvailable);
    context.DestroySemaphore(m_sRenderFinished); 
    context.DestroyCommandPool(m_CommandPool); 

    m_GUI.Shutdown(context);

    DestroySwapchain(context);
}

void Renderer::BeginFrame(const VKR::VkContext& context)
{
    //Synchronize 
    vkWaitForFences(context.GetDevice(), 1, &m_fFrameReady, VK_TRUE, UINT64_MAX);
    vkResetFences(context.GetDevice(), 1, &m_fFrameReady);

    vkAcquireNextImageKHR(context.GetDevice(), m_Swapchain.GetSwapchain(), UINT64_MAX, m_sImageAvailable, VK_NULL_HANDLE, &m_ImageIndex);

    //Allocate a new Command Buffer for this frame. 
    //TODO: Fix Race Condition!
    context.FreeCommandBuffers(m_CommandPool, 1, &m_CommandBuffer);
    context.AllocateCommandBuffers(m_CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &m_CommandBuffer);

    //Begin recording into the new command buffer. 
    const VkCommandBufferBeginInfo beginInfo = {
               VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
               nullptr,
               VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
               nullptr
    };
    vkBeginCommandBuffer(m_CommandBuffer, &beginInfo); 


    //Begin the render pass
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
            {0, 0, 1280, 720},   //TODO: Hook in Swapchain Extents
            3,
            clearValues
        };
        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    //Set Dynamic State
    vkCmdSetViewport(m_CommandBuffer, 0, 1, &m_Viewport);
    vkCmdSetScissor(m_CommandBuffer, 0, 1, &m_Scissor);

    //Draw the Grid
    //DrawGrid(); 
}

void Renderer::EndFrame()
{
    m_GUI.Update();
    m_GUI.Draw(m_CommandBuffer);

    vkCmdEndRenderPass(m_CommandBuffer);
    vkEndCommandBuffer(m_CommandBuffer); 

    //Submit our work to the GPU.
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    const VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        1,
        &m_sImageAvailable,
        waitStages,
        1,
        &m_CommandBuffer,
        1,
        &m_sRenderFinished
    };
    vkQueueSubmit(m_Queue, 1, &submitInfo, m_fFrameReady);

    //Present the Swapchain
    m_Swapchain.Present(m_Queue, m_sRenderFinished, &m_ImageIndex);

}

void Renderer::Draw(const Scene& scene)
{
}

void Renderer::DrawGrid()
{
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GridPipeline); 
    vkCmdDraw(m_CommandBuffer, 4, 1, 0, 0); 
}

void Renderer::CreateSwapchain(const VKR::VkContext& context, const VKR::Window& window)
{
    EASY_FUNCTION();
    Log::Message("Creating Swapchain.\n");
    //TODO: Release old resources if present. 

    const VkExtent2D extents = {
        window.GetWidth(),
        window.GetHeight()
    };

    m_Swapchain.Create(context, &window, m_QueueFamilyIndex);

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
        CreateColourRenderTarget(context, extents, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_MSAASamples, &m_RenderTargets[0]);  //Colour Attachment
        CreateDepthRenderTarget(context, extents, VkHelpers::FindDepthFormat(context.GetPhysicalDevice()), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_MSAASamples, &m_RenderTargets[1]);   //Depth Attachment
        CreateColourRenderTarget(context, extents, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SAMPLE_COUNT_1_BIT, &m_RenderTargets[2]);   //Resolve Attachment

        //Describe our Image Attachments.
        std::vector<VkAttachmentDescription> attachmentDescs(3);
        {
            attachmentDescs[0] = {
                0,
                VK_FORMAT_B8G8R8A8_UNORM,
                m_MSAASamples,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            attachmentDescs[1] = {
                0,
                VkHelpers::FindDepthFormat(context.GetPhysicalDevice()),
                m_MSAASamples,
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
        context.CreateRenderPass(attachmentDescs.size(), attachmentDescs.data(), 1, &subpassDesc, 1, &subpassDependency, &m_RenderPass);
    }

    //Create the Framebuffers
    {
        //Retrieve image view references for our colour attachments
        m_FrameBuffers.resize(m_Swapchain.GetImageCount());
        for (uint32_t i = 0; i < m_Swapchain.GetImageCount(); i++) {
            VkImageView attachments[3] = { m_RenderTargets[0].view, m_RenderTargets[1].view ,m_Swapchain.GetImageViews()[i] };
            context.CreateFrameBuffer({ extents.width, extents.height, 1 }, m_RenderPass, 3, attachments, &m_FrameBuffers[i]);
        }
    }

}

void Renderer::DestroySwapchain(const VKR::VkContext& context)
{
    for (uint32_t i = 0; i < m_Swapchain.GetImageCount(); i++) {
        context.DestroyFrameBuffer(m_FrameBuffers[i]);
    }

    context.DestroyRenderPass(m_RenderPass);
    for (uint32_t i = 0; i < m_RenderTargets.size(); i++) {
        DestroyImageResource(context, m_RenderTargets[i]);
    }

    m_Swapchain.Destroy(context);
}

void Renderer::CreateColourRenderTarget(const VKR::VkContext& context, const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource)
{
    EASY_FUNCTION();
    context.CreateImage(VK_IMAGE_TYPE_2D, { extents.width, extents.height, 1 }, MSAASamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VMA_MEMORY_USAGE_AUTO, 0, &pRenderTargetResource->allocation, &pRenderTargetResource->image);

    context.CreateImageView(pRenderTargetResource->image, format, VK_IMAGE_ASPECT_COLOR_BIT, &pRenderTargetResource->view);
}

void Renderer::CreateDepthRenderTarget(const VKR::VkContext& context, const VkExtent2D extents, const VkFormat format, const VkImageUsageFlagBits usage, const VkSampleCountFlagBits MSAASamples, ImageResource* pRenderTargetResource)
{
    EASY_FUNCTION();
    context.CreateImage(VK_IMAGE_TYPE_2D, { extents.width, extents.height, 1 }, MSAASamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VMA_MEMORY_USAGE_AUTO, 0, &pRenderTargetResource->allocation, &pRenderTargetResource->image);

    context.CreateImageView(pRenderTargetResource->image, format, VK_IMAGE_ASPECT_DEPTH_BIT, &pRenderTargetResource->view);
}

void Renderer::DestroyImageResource(const VKR::VkContext& context, ImageResource& resource)
{
    EASY_FUNCTION();
    context.DestroyImageView(resource.view);
    context.DestroyImage(resource.image, resource.allocation);
}

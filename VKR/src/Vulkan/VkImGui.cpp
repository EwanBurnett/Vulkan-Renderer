#include "../../include/VKR/Vulkan/VkImGui.h"
#include "../../include/VKR/Vulkan/VkContext.h"
#include "../../include/VKR/Logger.h"
#include "../../include/VKR/Window.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h> //TODO: remove this dependency.


VKR::VkImGui::VkImGui()
{
    m_Context = nullptr;
    m_pIO = nullptr;
    m_DescriptorPool = VK_NULL_HANDLE; 
}

void VKR::VkImGui::Init(const VkContext& context, const Window& window)
{
    //Initialize ImGui
    m_Context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_Context);

    m_pIO = &ImGui::GetIO();

    //Create a Descriptor Pool for ImGui
    const VkDescriptorPoolSize dpSizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    context.CreateDescriptorPool(1, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 11, dpSizes, &m_DescriptorPool);

    //Initialize Imgui in the Window
    ImGui_ImplGlfw_InitForVulkan(window.GLFWHandle(), true); 
}

void VKR::VkImGui::Shutdown(const VkContext& context)
{
    ImGui_ImplGlfw_Shutdown(); 
    ImGui_ImplVulkan_Shutdown(); 

    context.DestroyDescriptorPool(m_DescriptorPool);
    ImGui::DestroyContext(m_Context);
}

void VKR::VkImGui::BeginFrame()
{
    ImGui_ImplGlfw_NewFrame(); 
    ImGui_ImplVulkan_NewFrame(); 
    ImGui::NewFrame();
}

void VKR::VkImGui::EndFrame()
{
    ImGui::Render(); 
}

void VKR::VkImGui::Hook(const VkContext& context, const uint32_t queueFamilyIndex, const VkQueue queue, const VkPipelineCache pipelineCache, const uint32_t imageCount, const VkRenderPass renderPass, const VkSampleCountFlagBits samples)
{

    ImGui_ImplVulkan_InitInfo initInfo = {}; 
    initInfo.Instance = context.GetInstance(); 
    initInfo.PhysicalDevice = context.GetPhysicalDevice(); 
    initInfo.Device = context.GetDevice(); 
    initInfo.QueueFamily = queueFamilyIndex; 
    initInfo.Queue = queue; 
    initInfo.PipelineCache = pipelineCache; 
    initInfo.MinImageCount = imageCount;
    initInfo.ImageCount = imageCount; 
    initInfo.RenderPass = renderPass;
    initInfo.DescriptorPool = m_DescriptorPool;
    initInfo.MSAASamples = samples; 
    initInfo.Allocator = nullptr; 
    initInfo.CheckVkResultFn = nullptr;

    ImGui_ImplVulkan_Init(&initInfo); 
}

void VKR::VkImGui::Draw(VkCommandBuffer* pCmd)
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *pCmd);
}


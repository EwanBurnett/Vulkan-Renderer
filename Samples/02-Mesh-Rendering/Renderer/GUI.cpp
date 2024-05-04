#include "GUI.h"


void GUI::Init(const VKR::VkContext& context, const VKR::Window& window)
{
    m_ImGuiRenderer.Init(context, window);
}

void GUI::Shutdown(const VKR::VkContext& context)
{
    m_ImGuiRenderer.Shutdown(context); 
}

void GUI::Hook(const VKR::VkContext& context, const uint32_t queueFamilyIndex, const VkQueue queue, const uint32_t swapchainImageCount, const VkRenderPass renderPass, const VkSampleCountFlagBits samples)
{
    m_ImGuiRenderer.Hook(context, queueFamilyIndex, queue, VK_NULL_HANDLE, swapchainImageCount, renderPass, samples); 
}

void GUI::Update()
{
    m_ImGuiRenderer.BeginFrame();

    static bool bShowDemo = true; 
    ImGui::ShowDemoWindow(&bShowDemo); 

    m_ImGuiRenderer.EndFrame();
}

void GUI::Draw(VkCommandBuffer& cmd)
{
    m_ImGuiRenderer.Draw(&cmd); 
}

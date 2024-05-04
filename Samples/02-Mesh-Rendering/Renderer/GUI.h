#ifndef __GUI_H
#define __GUI_H

#include <VKR/Vulkan/VkImGui.h>

namespace VKR {
    class Context;
    class Window; 
}

class GUI {
public: 
    void Init(const VKR::VkContext& context, const VKR::Window& window); 
    void Shutdown(const VKR::VkContext& context);

    void Hook(const VKR::VkContext& context, const uint32_t queueFamilyIndex, const VkQueue queue, const uint32_t swapchainImageCount, const VkRenderPass renderPass, const VkSampleCountFlagBits samples); 

    void Update();
    void Draw(VkCommandBuffer& cmd); 
private:
    VKR::VkImGui m_ImGuiRenderer; 

};
#endif
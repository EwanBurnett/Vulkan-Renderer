#ifndef __VKRENDERER_VKIMGUI_H
#define __VKRENDERER_VKIMGUI_H
/**
*   @file VkImGui.h
*   @brief Vulkan ImGui Implementation 
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/24
*/
#include "VkCommon.h"
#include "../Maths.h"
#include <imgui.h>
#include <vector>

namespace VKR {
    class Window;
    class VkContext;
    class VkImGui {
    public:
        VkImGui(); 

        void Init(const VkContext& context, const Window& window); 
        void Shutdown(const VkContext& context); 

        void BeginFrame(); 

        void EndFrame(); 

        void Hook(const VkContext& context, const uint32_t queueFamilyIndex, const VkQueue queue, const VkPipelineCache pipelineCache, const uint32_t imageCount, const VkRenderPass renderPass, const VkSampleCountFlagBits samples); 

        void Draw(VkCommandBuffer* pCmd); 
    private:
        ImGuiContext* m_Context;
        ImGuiIO* m_pIO; 

        std::vector<ImFont> m_Fonts;
        std::vector<VkImage> m_FontImages;
        std::vector<VkImageView> m_FontImageViews;
        std::vector<VmaAllocation> m_FontImageAllocs; 

        VkDescriptorPool m_DescriptorPool;
         
    };
}

#endif
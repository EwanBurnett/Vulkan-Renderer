#include <VKR/VKR.h>
#include <VKR/Window.h>
#include <VKR/Logger.h>

#include <VKR/Vulkan/VkInit.h>
#include <VKR/Vulkan/VkHelpers.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector> 
#include "Renderer/Renderer.h"

constexpr uint32_t WINDOW_WIDTH = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;

void InitVulkanContext(VKR::VkContext& context); 
void ShutdownVulkanContext(VKR::VkContext& context); 

int main() {
    VKR::Init();

    //EASY_PROFILER_ENABLE;
    
    VKR::VkContext vkContext; 
    InitVulkanContext(vkContext); 
    
    //Create a Window. 
    VKR::Window window;
    window.Create("VKR Sample 02 - Mesh Rendering", WINDOW_WIDTH, WINDOW_HEIGHT);

    //Initialize the Renderer
    Renderer renderer; 
    renderer.Init(vkContext, window);

    //Set the window icon.
    {
        GLFWimage icon;
        icon.pixels = stbi_load("Data/VKR_Icon.png", &icon.width, &icon.height, nullptr, 0);
        glfwSetWindowIcon(window.GLFWHandle(), 1, &icon);
    }


    //Run the Render Loop
    while (window.PollEvents()) {
        //TODO: Update Application State
        renderer.BeginFrame(vkContext); 

        renderer.EndFrame(); 
    }
    printf("\n");   //Print a newline for correct log output. 

    window.Hide();

    window.Destroy();
    renderer.Shutdown(vkContext); 

    ShutdownVulkanContext(vkContext); 

    VKR::Shutdown();
    return 0;
}

void InitVulkanContext(VKR::VkContext& context)
{
    //Vulkan Instance Creation
    VKR::Log::Message("Creating Vulkan Instance.\n");
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

        VK_CHECK(context.CreateInstance(instanceLayers.size(), instanceLayers.data(), instanceExtensions.size(), instanceExtensions.data(), nullptr));
    }

#if DEBUG
    VKR::Log::Message("Creating Vulkan Debug Objects.\n");
    //Vulkan Debug Callback Object Creation
    {
        context.CreateDebugLogger();
        context.CreateDebugReporter();
    }
#endif

    VKR::Log::Message("Creating Vulkan Device.\n");
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

        VK_CHECK(context.SelectPhysicalDevice());

        const uint32_t queueFamilyIndex = VKR::VkHelpers::FindQueueFamilyIndex(context.GetPhysicalDevice(), VK_QUEUE_GRAPHICS_BIT);

        float queuePriorities[] = { 1.0f };
        const VkDeviceQueueCreateInfo qci = VKR::VkInit::MakeDeviceQueueCreateInfo(queueFamilyIndex, 1, queuePriorities);

        VkPhysicalDeviceFeatures features = {};
        features.pipelineStatisticsQuery = true;
        context.CreateDevice(deviceExtensions.size(), deviceExtensions.data(), 1, &qci, &features);
    }

    context.CreateAllocator();
}

void ShutdownVulkanContext(VKR::VkContext& context)
{
    VKR::Log::Message("Destroying Vulkan Context.\n");
#if DEBUG
    context.DestroyDebugReporter();
    context.DestroyDebugLogger();
#endif

    context.DestroyAllocator();
    context.DestroyDevice();

    context.DestroyInstance();
}

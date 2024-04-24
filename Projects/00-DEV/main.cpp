#include <VKR.h>
#include <Window.h>
#include <Timer.h>
#include <Maths.h>

#include <Vulkan/VkContext.h>

#include <vector> 
#include <Thread>


#include <cstdio> 
#include <easy/profiler.h>

constexpr uint32_t WINDOW_WIDTH = 600;
constexpr uint32_t WINDOW_HEIGHT = 400;

void InitVulkan(VKR::VkContext& context);
void ShutdownVulkan(VKR::VkContext& context);

int main() {
   
    EASY_BLOCK("App Initialization");
    VKR::Init(); 
    
    VKR::VkContext context; 
    VKR::Window window;
    window.Create("Vulkan Renderer", WINDOW_WIDTH, WINDOW_HEIGHT);
    window.Show();

    InitVulkan(context); 

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
            //std::this_thread::sleep_for(std::chrono::milliseconds(7));  //Sleep to simulate some work
            printf("\r                                                          "); //Clear the current line for consistent output
            printf("\rFrame %d\tdtms: %04fms\t%d fps\truntime: %fs", frameIdx++, dtms, fps, runtime);
        }
    }

    EASY_BLOCK("App Shutdown");

    printf("\n");   //Print a newline for correct logging
    window.Hide();
    window.Destroy();

    ShutdownVulkan(context); 

    EASY_END_BLOCK;

    VKR::Shutdown(); 
   
    return 0;
}

//--------------------------

void InitVulkan(VKR::VkContext& context) {

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
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME
    };


    VK_CHECK(context.CreateInstance(instanceLayers.size(), instanceLayers.data(), instanceExtensions.size(), instanceExtensions.data(), nullptr));
#if DEBUG
    VK_CHECK(context.CreateDebugLogger());
    VK_CHECK(context.CreateDebugReporter());
#endif
}



void ShutdownVulkan(VKR::VkContext& context) {
#ifdef DEBUG
    context.DestroyDebugReporter(); 
    context.DestroyDebugLogger(); 
#endif

    context.DestroyInstance(); 
}


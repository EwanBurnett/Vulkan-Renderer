#include <Renderer.h>
#include <Window.h>
#include <Timer.h>
#include <Maths.h>

#include <Thread>
#include <cstdio> 
#include <easy/profiler.h>

constexpr uint32_t WINDOW_WIDTH = 600; 
constexpr uint32_t WINDOW_HEIGHT = 400; 

int main() {
    glfwInit(); 
    EASY_MAIN_THREAD; 
    EASY_PROFILER_ENABLE; 

    EASY_BLOCK("App Initialization")

    VKR::Window window; 
    window.Create("Vulkan Renderer", WINDOW_WIDTH, WINDOW_HEIGHT);
    window.Show(); 

    VKR::Renderer renderer; 
    renderer.Init(); 

    VKR::Timer timer; 
    timer.Start(); 

    uint64_t frameIdx = 0;  //Keep track of the current frame. 
    double runtime = 0; 
    uint64_t fps = 0; 
    double dtms = 0.0; 

    EASY_END_BLOCK;


    while (window.PollEvents()) {
        EASY_BLOCK("Main Loop", profiler::colors::SkyBlue);
        timer.Tick(); 
        dtms = timer.DeltaTime(); 
        fps = 1.0 / dtms;
        runtime = timer.Duration(); 
        
        std::this_thread::sleep_for(std::chrono::milliseconds(7));  //Sleep to simulate some work
        printf("\r                                                          "); //Clear the current line for consistent output
        printf("\rFrame %d\tdtms: %04fms\t%d fps\truntime: %fs", frameIdx++, dtms, fps, runtime);
    }

    EASY_BLOCK("App Shutdown")

    renderer.Shutdown(); 
    window.Destroy(); 

    glfwTerminate(); 

    EASY_END_BLOCK; 

    profiler::dumpBlocksToFile("PROFILE_RESULT.prof");

    return 0; 
}
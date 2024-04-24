#include <Renderer.h>
#include <Window.h>
#include <Timer.h>
#include <Maths.h>

#include <vector> 
#include <Thread>

#include <cstdio> 
#include <easy/profiler.h>

constexpr uint32_t WINDOW_WIDTH = 600;
constexpr uint32_t WINDOW_HEIGHT = 400;

int main() {
    glfwInit();
    EASY_MAIN_THREAD;
    EASY_PROFILER_ENABLE; //TODO: Debug Switch
    profiler::startListen();

    EASY_BLOCK("App Initialization");

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
    renderer.Shutdown();
    window.Destroy();

    glfwTerminate();

    EASY_END_BLOCK;

    profiler::stopListen();
    //profiler::dumpBlocksToFile("PROFILE_RESULT.prof");

    return 0;
}
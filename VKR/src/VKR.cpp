#include "../include/VKR/VKR.h"
#include "../include/VKR/Logger.h"
#include <easy/profiler.h>
#include <GLFW/glfw3.h>

VKR::Status VKR::Init()
{
    EASY_FUNCTION(profiler::colors::Grey600);

    EASY_MAIN_THREAD;
    PROFILER_START_LISTENING;

    //If we fail to initialize GLFW, terminate. 
    if (glfwInit() != GLFW_TRUE) {
        return Status::FAILED; 
    }
    
    return Status::SUCCESS; 
}


VKR::Status VKR::Shutdown()
{
    EASY_FUNCTION(profiler::colors::Grey600);
    
    glfwTerminate();
    PROFILER_STOP_LISTENING;

    return Status::SUCCESS;
}


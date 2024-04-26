#include "VKR.h"
#include "../include/Logger.h"
#include <easy/profiler.h>
#include <GLFW/glfw3.h>

VKR::Status VKR::Init()
{
    EASY_FUNCTION(profiler::colors::Grey600);
    
    glfwInit();
    EASY_MAIN_THREAD;
    //EASY_PROFILER_ENABLE; //TODO: Debug Switch
    profiler::startListen();

    return Status::NOT_IMPLEMENTED; 
}


VKR::Status VKR::Shutdown()
{
    EASY_FUNCTION(profiler::colors::Grey600);
    
    glfwTerminate();
    //profiler::stopListen();
    //profiler::dumpBlocksToFile("PROFILE_RESULT.prof");

    return Status();
}


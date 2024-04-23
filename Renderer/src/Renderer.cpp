#include "../include/Renderer.h"
#include "../include/Logger.h"
#include <cstdio> 
#include <easy/profiler.h>

VKR::Status VKR::Renderer::Init() {
    EASY_FUNCTION(profiler::colors::Red200);

    Log::Message("[VKR]\tInitializing Renderer.\n");

    return SUCCESS; 
}

VKR::Status VKR::Renderer::Shutdown() {
    EASY_FUNCTION(profiler::colors::Red200); 
    Log::Message("[VKR]\tShutting Down Renderer.\n");

    return SUCCESS;
}
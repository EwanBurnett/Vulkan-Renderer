#include "../include/Renderer.h"
#include "../include/Logger.h"
#include <cstdio> 
#include <easy/profiler.h>

VKR::Status VKR::Renderer::Init() {
    EASY_FUNCTION(profiler::colors::Red200);

    Log::Message("[VKR]\tInitializing Renderer.\n");

    
    for (uint64_t i = 0; i < 5472458; i++) {

    }
    return SUCCESS; 
}

VKR::Status VKR::Renderer::Shutdown() {
    EASY_FUNCTION(profiler::colors::Red200); 
    Log::Message("[VKR]\tShutting Down Renderer.\n");

    return SUCCESS;
}
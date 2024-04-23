#include "../include/Renderer.h"
#include "../include/Logger.h"
#include <cstdio> 

VKR::Status VKR::Renderer::Init() {
    Log::Message("[VKR]\tInitializing Renderer.\n");

    return SUCCESS; 
}

VKR::Status VKR::Renderer::Shutdown() {
    Log::Message("[VKR]\tShutting Down Renderer.\n");

    return SUCCESS;
}
#ifndef __VKRENDERER_H
#define __VKRENDERER_H
/**
*   @file Renderer.h
*   @brief Renderer Abstraction Layer
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/23
*/

#include <vulkan/vulkan.h>
#include "Types.h"

namespace VKR {

    class Renderer {
    public:
        Status Init();
        Status Shutdown();
    };

}
#endif
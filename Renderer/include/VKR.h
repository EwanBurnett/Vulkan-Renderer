#ifndef __VKRENDERER_H
#define __VKRENDERER_H
/**
*   @file VKR.h
*   @brief Rendering Engine Initialization Layer
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/24
*/

#include "Types.h"

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__ 
#endif

namespace VKR {
    Status Init();
    Status Shutdown();
}
#endif
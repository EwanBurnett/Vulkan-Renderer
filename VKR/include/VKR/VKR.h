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

#ifndef VKR_DEBUG
#define EASY_FUNCTION(...)
#define EASY_BLOCK(...)
#define EASY_END_BLOCK
#define EASY_THREAD_SCOPE(...)
#define EASY_PROFILER_ENABLE
#define EASY_MAIN_THREAD
#define PROFILER_START_LISTENING
#define PROFILER_STOP_LISTENING
#define PROFILER_DUMP(filename)
#else
#define PROFILER_START_LISTENING profiler::startListen()
#define PROFILER_STOP_LISTENING profiler::stopListen()
#if VKR_PROFILER_RECORD
#define PROFILER_DUMP(filename) profiler::dumpBlocksToFile(filename)
#endif
#endif

namespace VKR {
    /**
     * @brief Initializes VKR. 
     * @return 
    */
    Status Init();

    /**
     * @brief Shuts down VKR. 
     * @return 
    */
    Status Shutdown();
}
#endif
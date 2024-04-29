#ifndef __VKR_SAMPLES_DEMOAPP_H
#define __VKR_SAMPLES_DEMOAPP_H
/**
*   @file DemoApp.h
*   @brief Common Framework for VKR Sample Applications.
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/28
*/
#include <cstdint> 

namespace VKR {
    class Window;
}

namespace Samples {
    /**
     * @brief Represents a Demo Application.
    */
    class DemoApp {
    public:
        DemoApp();
        virtual void Init(const VKR::Window& window) = 0;
        virtual void Update() = 0;
        virtual void FixedUpdate() = 0;
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Shutdown() = 0;

    protected:
        double m_RunTime;
        double m_DeltaTime;
        uint64_t m_FPS;
        uint64_t m_FrameCount;

    };
}

#endif
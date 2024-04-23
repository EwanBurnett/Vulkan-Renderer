#ifndef __VKRENDERER_TIMER_H
#define __VKRENDERER_TIMER_H
/**
*   @file Timer.h
*   @brief Performance Timing Utility
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/23
*/
#include <chrono>

namespace VKR {
    /**
     * @brief Times the period between calls to Tick(). Start() must be called to count time.  
    */
    class Timer {
    public:
        Timer(); 

        template<typename period = std::chrono::nanoseconds>
        double DeltaTime() const;

        template<typename period = std::chrono::nanoseconds> 
        double Duration() const; 

        void Tick(); 

        void Start(); 
        void Stop(); 
        void Reset(); 
    private:
        bool m_bIsRunning; 

        std::chrono::high_resolution_clock::time_point m_tPrevious;
        std::chrono::high_resolution_clock::time_point m_tCurrent;

        std::chrono::high_resolution_clock::duration m_Duration; 
    };


    template<typename period>
    inline double VKR::Timer::DeltaTime() const
    {
        static_assert(std::chrono::_Is_duration_v<period>, "Template argument <period> Must be a std::chrono::duration!\n");

        return std::chrono::duration_cast<period>(m_tCurrent - m_tPrevious).count() / (double)period::period::den;
    }


    template<typename period>
    inline double Timer::Duration() const
    {
        static_assert(std::chrono::_Is_duration_v<period>, "Template argument <period> Must be a std::chrono::duration!\n");
        return m_Duration.count() / (double)period::period::den;
    }
}

#endif
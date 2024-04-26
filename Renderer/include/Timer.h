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

        /**
         * @brief Returns the Interval between subsequent calls to Tick(). 
         * @tparam period the resolution of DeltaTime. 
         * @return 
        */
        template<typename period = std::chrono::nanoseconds>
        double DeltaTime() const;

        /**
         * @brief Returns the total running duration of this timer. 
         * @tparam period 
         * @return 
        */
        template<typename period = std::chrono::nanoseconds> 
        double Duration() const; 

        /**
         * @brief Updates the timer. 
        */
        void Tick(); 

        /**
         * @brief 
        */
        void Start();

        /**
         * @brief 
        */
        void Stop(); 

        /**
         * @brief 
        */
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
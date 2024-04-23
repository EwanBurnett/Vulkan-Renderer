#include "../include/Timer.h"

VKR::Timer::Timer() {
    m_bIsRunning = false;
    Reset();
}

void VKR::Timer::Tick()
{
    if (m_bIsRunning) {
        m_tPrevious = m_tCurrent;
        m_tCurrent = std::chrono::high_resolution_clock::now();

        m_Duration += (m_tCurrent - m_tPrevious);
    }
}

void VKR::Timer::Start()
{
    m_bIsRunning = true; 
}

void VKR::Timer::Stop()
{
    m_bIsRunning = false; 
}

void VKR::Timer::Reset()
{
    m_tPrevious = std::chrono::high_resolution_clock::now();
    m_tCurrent = std::chrono::high_resolution_clock::now();

    m_Duration = {};
}

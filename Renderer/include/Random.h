#ifndef __VKRENDERER_RANDOM_H
#define __VKRENDERER_RANDOM_H

/**
*   @file Random.h
*   @brief Random Number Generation Utility
*   @author Ewan Burnett (EwanBurnettSK@outlook.com)
*   @date 2024/04/23
*/

#include <random>
#include <chrono>

namespace VKR {
    class RNG {
    public:
        RNG(const uint64_t seed = 0);
        
        template <typename T = double>
        T Get(const T min = 0.0, const T max = 1.0);

    private:
        std::mt19937 m_RNG;
    };

    template<typename T>
    inline T RNG::Get(const T min, const T max)
    {
        static_assert(std::is_arithmetic<T>(), "Random Number Type was not Arithmetic!\n");
        if constexpr (std::is_integral<T>()) {
            static std::uniform_int_distrubution<> range(min, max);
            return range(m_RNG);
        }
        else if constexpr (std::is_floating_point<T>()) {
            static std::uniform_real_distribution<> range(min, max);    //This range may be used quite heavily, so statically store the distribution. 
            return range(m_RNG);
        }
    }
}

#endif
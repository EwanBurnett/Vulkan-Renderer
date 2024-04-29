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
    /**
     * @brief Random Number Generator Utility Class.
    */
    class RNG {
    public:
        /**
         * @brief Initializes the Random Number Generator. 
         * @param seed optional RNG seed 
        */
        RNG(const uint64_t seed = 0);
        
        /**
         * @brief Retrieve a random number. 
         * @tparam T Arithmetic type for the Random number generator. 
         * @param min minimum number to return. 0.0 by default. 
         * @param max maximum number to return. 1.0 by default. 
         * @return a random number between min and max. 
        */
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
            static std::uniform_int_distribution<> range(min, max);
            return range(m_RNG);
        }
        else if constexpr (std::is_floating_point<T>()) {
            static std::uniform_real_distribution<> range(min, max);    //This range may be used quite heavily, so statically store the distribution. 
            return range(m_RNG);
        }
    }
}

#endif

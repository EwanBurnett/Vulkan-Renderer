#include "../include/VKR/Random.h"

VKR::RNG::RNG(const uint64_t seed) {
    if (seed == 0) {
        std::random_device rd;
        m_RNG = std::mt19937(rd()); //Seed using a hardware generated random number    
    }
    else {
        m_RNG.seed(seed);
    }
}

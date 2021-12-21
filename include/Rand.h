#ifndef __RAND_H__
#define __RAND_H__

#include <random>
#include <math.h>

typedef std::mt19937 RNG;

class Rand{

    private:
        //static std::random_device _rd;
        inline static RNG _rng = RNG(std::random_device("/dev/random")());
        inline static std::uniform_real_distribution<> _theta_rand = std::uniform_real_distribution<>(-M_PI,M_PI);
        inline static std::uniform_real_distribution<> _unit_rand = std::uniform_real_distribution<>(0,1);

    public:
        static double theta_rand(){
            return _theta_rand(_rng);
        }
        static double unit_rand(){
            return _unit_rand(_rng);
        }
};

//RNG Rand::_rng = RNG(_rd());

#endif
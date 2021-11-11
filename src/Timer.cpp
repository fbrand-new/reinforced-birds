#include "Timer.h"

Timer::Timer():
    _start(std::chrono::high_resolution_clock::now())
    {}

Timer::~Timer(){
    stop();
}

void Timer::stop(){

    auto endtime = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(_start).time_since_epoch().count();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endtime).time_since_epoch().count();

    auto duration = end-start;
    std::cout << duration << " mics" << std::endl;
}
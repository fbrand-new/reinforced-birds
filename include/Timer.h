#include <chrono>
#include <iostream>

class Timer{

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> _start;

    public:
        Timer();
        ~Timer();
        void stop();
};
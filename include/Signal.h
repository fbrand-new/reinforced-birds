#ifndef __SIGNAL_H_
#define __SIGNAL_H_

#include <vector>

class Signal{

    private:
        std::vector<std::size_t> _time_of_step;

    public:
        Signal(std::vector<std::size_t> t);
        //Signal(std::vector<std::size_t> &&t);
        //For now let's return a step function
        bool step(std::size_t t);
};

#endif
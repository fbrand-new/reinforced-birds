#ifndef __SIGNAL_H_
#define __SIGNAL_H_

#include <vector>

class Signal{

    //Used to generate a time signal to alternate the learning process between preys and predator

    private:
        std::vector<std::size_t> _time_of_step;

    public:
        Signal(std::vector<std::size_t> t);
        bool step(std::size_t t);
        std::size_t multistep(std::size_t t, std::size_t max_step);
};

#endif
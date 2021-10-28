#include "Signal.h"

Signal::Signal(std::vector<std::size_t> t):
    _time_of_step{t}
    {}

bool Signal::step(std::size_t t){

    auto s = this->_time_of_step.size();

    for(std::size_t i=0; i<s; ++i){
        if(t < this->_time_of_step[i]){
            return (i%2 == 0);
            break;
        }
    }

    return (s-1)%2;
}
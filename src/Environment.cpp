#include "Environment.h"

Environment::Environment(unsigned int birds_num):
    _state(birds_num)
    {}

//TODO: we should pass a vector of actions, and the indexes of the affected birds!
//
State Environment::dynamics(Action a, const State& s){

    double alpha_new = 10*M_PI/180;

    State new_state;

    switch(a){
        case Action::left:
            //std::cout << "Left" << std::endl;
            s.update(_v0, -alpha_new);
            break;
        case Action::straight:
            //std::cout << "Straight" << std::endl;
            s.update(_v0, 0);
            break;
        case Action::right:
            //std::cout << "Right" << std::endl;
            s.update(_v0, alpha_new);
            break;
    }

    return s;
}
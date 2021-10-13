#include "Environment.h"

Environment::Environment(unsigned int birds_num):
    _state(birds_num)
    {}

//TODO: we should pass a vector of actions, and the indexes of the affected birds!
//
State Environment::dynamics(std::vector<Action> a, State& s){

    double alpha_new = 10*M_PI/180;
    //State new_state(s.size());

    for(std::size_t i=0; i<s.size(); ++i){
        switch(a[i]){
            case Action::left:
                //std::cout << "Left" << std::endl;
                s.update(_v0, -alpha_new, i);
                break;
            case Action::straight:
                //std::cout << "Straight" << std::endl;
                s.update(_v0, 0, i);
                break;
            case Action::right:
                //std::cout << "Right" << std::endl;
                s.update(_v0, alpha_new, i);
                break;
        }
    }
    return s;
}
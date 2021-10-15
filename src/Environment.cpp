#include "Environment.h"

Environment::Environment(unsigned int birds_num):
    _state(birds_num),
    _v0{0.1},
    _capture_range{0.1}
    {}

Environment::Environment(std::size_t birds_num, double v0, double capture_range):
    _state(birds_num),
    _v0{v0},
    _capture_range{capture_range}
    {}

//TODO: we should pass a vector of actions, and the indexes of the affected birds!
//
State Environment::dynamics(std::vector<Action> a, State& s){

    double alpha_new = 90*M_PI/180;
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

Reward Environment::reward(State &s){

    Reward r(s.size());
    auto predator = s.get_birds()[0];

    //By default, we get a negative reward to predator
    r[0] = -1;

    for(std::size_t i=1; i<s.size(); ++i){
        if(Agent::relative_distance(predator, s.get_birds()[i]) < _capture_range){
            r[0] = 1;
            break;
        } 
        r[i] = 1;
    }

    return r;
}
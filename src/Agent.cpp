#include "Agent.h"

void Agent::obs(State &s){

    //TODO: clearly these should be put in the constructor
    //And be parameterizable
    double vision_range = 2;
    double vision_angle = 120;

    auto birds = s.get_birds();
    std::size_t me = this->get_id();

    for(std::size_t i=0; i < this->_o.get_sectors_num(); ++i){
        for(std::size_t k=0; k<birds.size(); ++k){
            if(k != me){
                if (relative_distance(birds[me], birds[k]) > vision_range)
                    continue;
                else if (abs(relative_angle(birds[me], birds[k])) < vision_angle/2)
                    _o.non_empty_sector(i);
            }
        }     
    }
}

Action Agent::act(State &s){

    obs(s);
    return _p.decide(_o);
}

double Agent::relative_distance(Bird &a, Bird &b){

    return sqrt(pow(a.get_x() - b.get_x(),2) + pow(a.get_y() - b.get_y(),2));
}

double Agent::relative_angle(Bird &a, Bird &b){

    return atan(abs(a.get_y() - b.get_y())/abs(a.get_x() - b.get_x())) *180 /M_PI;
}
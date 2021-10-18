#include "Agent.h"

Agent::Agent():
    _p{std::make_unique<Policy>()},
    _vision_range{10},
    _vision_angle{270}
    {}

// Agent::Agent(std::size_t id, Policy &p):
//     _id{id},
//     _p{p}
//     {}

Agent::Agent(Policy &p):
    _p{std::make_unique<Policy>(p)}
    {}

Agent::Agent(double vision_range, double vision_angle):
    _p{std::make_unique<Policy>()},
    _vision_range{vision_range},
    _vision_angle{vision_angle}
    {}

Observable Agent::obs(State &s){

    //TODO: clearly these should be put in the constructor
    //And be parameterizable
    std::size_t sectors_num = this->_o.get_sectors_num();

    double vision_range = 2;
    double vision_angle = 180;

    double vision_sector_min = -_vision_angle/2;
    double vision_sector_max = vision_sector_min+ _vision_angle/sectors_num;

    auto birds = s.get_birds();
    std::size_t me = this->get_id();

    for(std::size_t i=0; i <sectors_num; ++i){

        //reset sector
        _o.empty_sector(i);
        for(std::size_t k=0; k<birds.size(); ++k){
            if(k != me){
                if (relative_distance(birds[me], birds[k]) > vision_range)
                    continue;
                else if (relative_angle(birds[me], birds[k]) < vision_sector_max){
                    if(relative_angle(birds[me], birds[k]) > vision_sector_min)
                        _o.non_empty_sector(i);
                }
            }
        }     
        vision_sector_min = vision_sector_max;
        vision_sector_max = vision_sector_min+vision_angle/sectors_num;
    }

    return _o;
}

Action Agent::act(State &s, Observable &o){

    return _p->decide(o);
}

void Agent::update_policy(double coeff, Observable &o, Action &a){

    _p->update(coeff, o, a);
}

double Agent::relative_distance(const Bird &a, const Bird &b){

    return sqrt(pow(a.get_x() - b.get_x(),2) + pow(a.get_y() - b.get_y(),2));
}

double Agent::relative_angle(const Bird &a, const Bird &b){

    return atan(abs(a.get_y() - b.get_y())/abs(a.get_x() - b.get_x())) *180 /M_PI;
}
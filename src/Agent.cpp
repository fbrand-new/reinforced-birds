#include "Agent.h"

Agent::Agent():
    _p{std::make_unique<Policy>()},
    _vision_range{100},
    _vision_angle{M_PI}
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

    std::size_t sectors_num = this->_o.get_sectors_num();
    auto birds = s.get_birds();
    std::size_t me = this->get_id();

    //double alpha = birds[me].get_alpha();
    //This is the relative angle with respect to the agent orientation
    //double delta = abs(relative_angle(birds[me], birds[k]) -  birds[me].get_alpha()); 

    //The vision is related to the orientation of the bird
    double vision_sector_min = -_vision_angle/2;
    double vision_sector_max = vision_sector_min +  _vision_angle/sectors_num;

    //Resetting observation
    for(std::size_t k=0; k<sectors_num; ++k){
        _o.empty_sector(k);
    }

    for(std::size_t i=0; i <sectors_num; ++i){
        for(std::size_t k=0; k<birds.size(); ++k){
            if(k != me){
                if (relative_distance(birds[me], birds[k]) > _vision_range)
                    continue;              
                else if (relative_angle(birds[me], birds[k]) < vision_sector_max){
                    if(relative_angle(birds[me], birds[k]) > vision_sector_min)
                        _o.non_empty_sector(i);
                }
            }
        }     
        vision_sector_min = vision_sector_max;
        vision_sector_max = vision_sector_min+ _vision_angle/sectors_num;
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

    double alpha = a.get_alpha();

    //Centering the system on Bird a
    double x_rel = b.get_x() - a.get_x();
    double y_rel = b.get_y() - a.get_y();

    //Counter-clockwise rotation of the system towards Bird a orientation
    double x_prime = cos(-alpha)*x_rel - sin(-alpha)*y_rel;
    double y_prime = sin(-alpha)*x_rel + cos(-alpha)*y_rel;

    return atan2(y_prime, x_prime);
}

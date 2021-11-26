#include "Environment.h"

Environment::Environment(std::size_t birds_num):
    _state(birds_num),
    _v0{0.1},
    _capture_range{0.1},
    _steering_angle{M_PI/6}
    {}

Environment::Environment(std::size_t birds_num, double v0, double capture_range, double steering_angle):
    _state(birds_num),
    _v0{v0},
    _capture_range{capture_range},
    _steering_angle{steering_angle}
    {}


State Environment::dynamics(std::vector<Action> a, State& s){

    double alpha_new = _steering_angle;

    for(std::size_t i=0; i<s.size(); ++i){
        
        double v;
        //Slight speed advantage to evader -> This leads to linear evasion. Game over
        //Slight speed advantage to pursuer
        if(i==0){
            v = _v0*2;
        } else {
            v = _v0;
        }
        //v=_v0;
        switch(a[i]){
            case Action::left:
                //std::cout << "Left" << std::endl;
                s.update(v, +alpha_new, i);
                break;
            case Action::straight:
                //std::cout << "Straight" << std::endl;
                s.update(v, 0, i);
                break;
            case Action::right:
                //std::cout << "Right" << std::endl;
                s.update(v, -alpha_new, i);
                break;
        }
    }
    return s;
}

std::pair<Reward,bool> Environment::reward(State &s, double episode_length, int num_of_preys){

    Reward r(s.size());
    bool episode_over = 0;

    auto predator = s.get_birds()[0];

    //By default, we get a negative reward to predator
    r[0] = -1;

    for(std::size_t i=1; i<s.size(); ++i){
        if(relative_distance_squared(predator, s.get_birds()[i]) < _capture_range*_capture_range){
            // r[0] = episode_length;
            // r[i] = -episode_length;
            r[0] = episode_length;
            r[i] = -episode_length;
            episode_over=1;
        } else{
            r[i] = 1./(num_of_preys);
        }
    }

    // if(episode_over==1){
    //     for(auto i =0; i<s.size(); ++i)
    //         std::cout << r[i] << std::endl;
    // }


    return std::make_pair(r,episode_over);
}

void Environment::reset(){

    _state.reset();
}
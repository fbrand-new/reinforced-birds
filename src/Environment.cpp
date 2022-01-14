#include "Environment.h"

Environment::Environment(std::size_t birds_num):
    _state(birds_num),
    _v0(birds_num,0.1),
    _capture_range{0.1},
    _steering_angles(birds_num,M_PI/6)
    {}

Environment::Environment(std::size_t birds_num, double v0, double capture_range, double steering_angle):
    _state(birds_num),
    _v0(birds_num,v0),
    _capture_range{capture_range},
    _steering_angles(birds_num,steering_angle)
    {}

Environment::Environment(std::size_t birds_num, double v_pursuer, double v_evader, double capture_range, double steering_angle_pursuer, double steering_angle_evader):
    _state(birds_num),
    _v0(birds_num,v_evader),
    _capture_range{capture_range},
    _steering_angles(birds_num,steering_angle_evader)
    {
        _v0[0] = v_pursuer;
        _steering_angles[0] = steering_angle_pursuer;
    }


State Environment::dynamics(std::vector<Action> a, State& s){

    for(std::size_t i=0; i<s.size(); ++i){
        
        switch(a[i]){
            case Action::left:
                //std::cout << "Left" << std::endl;
                s.update(_v0[i], _steering_angles[i], i);
                break;
            case Action::straight:
                //std::cout << "Straight" << std::endl;
                s.update(_v0[i], 0, i);
                break;
            case Action::right:
                //std::cout << "Right" << std::endl;
                s.update(_v0[i], -_steering_angles[i], i);
                break;
        }
    }
    return s;
}

std::pair<Reward,bool> Environment::reward(State &s, double episode_length, int num_of_preys){

    Reward r(s.size());
    bool episode_over = 0;
    auto friends_range = 1.5;

    auto birds = s.get_birds();
    auto predator = birds[0];

    //By default, we get a negative reward to pursuer and a positive one to evader
    r[0] = -1;
    for(std::size_t i=1; i<s.size(); ++i){
        r[i] = 1./num_of_preys;
    }

    for(std::size_t i=1; i<s.size(); ++i){
        if(relative_distance_squared(predator, birds[i]) < _capture_range*_capture_range){
            r[0] = episode_length;
            r[i] = -episode_length;
            // r[0] = 2;
            // r[i] = -1;
            // for(std::size_t j=1; j < s.size(); ++j){
            //     if(j != i)
            //         r[j] = -1./(num_of_preys-1); //each one takes a small penalty if one gets caught. Let's see.
            // }
            episode_over=1;
        // } else{
        //     r[i] = 1./(num_of_preys);
        // }
        } else {
            for(std::size_t k=i+1; k<s.size(); ++k){
                if(relative_distance_squared(birds[i],birds[k]) < friends_range*friends_range){
                    r[i] = -1;
                    r[k] = -1;
                    break;
                }
            }
        }
    }

    return std::make_pair(r,episode_over);
}

void Environment::reset(){

    _state.reset();
}
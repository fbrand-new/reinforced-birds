#include "Environment.h"

Environment::Environment(std::size_t birds_num, double v_pursuer, double v_evader,
                         double friends_range, double capture_range, double steering_angle_pursuer, 
                         double steering_angle_evader, double pbc, double pursuer_vis_range,
                         std::pair<Angle,Angle> pursuer_vis_angle, double av_init_dist, double prey_repulsion,
                         double attraction_range, double attraction_reward):
    _state(birds_num, pbc,av_init_dist),
    _v0(birds_num,v_evader),
    _friends_range{friends_range},
    _capture_range{capture_range},
    _steering_angles(birds_num,steering_angle_evader),
    _pbc{pbc},
    _pursuer_vis_range{pursuer_vis_range},
    _pursuer_vis_angle{pursuer_vis_angle},
    _prey_repulsion{prey_repulsion},
    _attraction_range{attraction_range},
    _attraction_reward{attraction_reward}
    {
        _v0[0] = v_pursuer;
        _steering_angles[0] = steering_angle_pursuer;
    }


State Environment::dynamics(std::vector<Action> a, State& s){

    //Defines the dynamics based on the action taken by each bird
    for(std::size_t i=0; i<s.size(); ++i){
        
        switch(a[i]){
            case Action::left:
                s.update(_v0[i], _steering_angles[i], i);
                break;
            case Action::straight:
                s.update(_v0[i], 0, i);
                break;
            case Action::right:
                s.update(_v0[i], -_steering_angles[i], i);
                break;
        }
    }
    return s;
}

std::pair<Reward,bool> Environment::reward(State &s, double episode_length, int num_of_preys){

    //Assigns rewards based on the state of the episode.

    Reward r(s.size());
    bool episode_over = 0;

    auto birds = s.get_birds();
    auto predator = birds[0];
    auto pbc = s.get_pbc();

    //By default, we get a negative reward to pursuer and a positive one to evader
    r[0] = -1;
    for(std::size_t i=1; i<s.size(); ++i){
        r[i] = 1./num_of_preys;
    }

    for(std::size_t i=1; i<s.size(); ++i){

        //If prey i is caught
        if(relative_distance_squared(predator, birds[i],pbc) < _capture_range*_capture_range){
            r[0] = episode_length;
            r[i] = -episode_length;
            episode_over=1;
        } else {
            for(std::size_t k=i+1; k<s.size(); ++k){
                //If prey k and prey i are too close
                if(relative_distance_squared(birds[i],birds[k],pbc) < _friends_range*_friends_range){
                    r[i] = -_prey_repulsion;
                    r[k] = -_prey_repulsion;
                    break;
                }
                else if(relative_distance_squared(birds[i],birds[k],pbc) < _attraction_range*_attraction_range){
                    r[i] += _attraction_reward;
                    r[k] += _attraction_reward;
                }
            }
        }
    }

    return std::make_pair(r,episode_over);
}

void Environment::reset(){

    _state.reset();
}
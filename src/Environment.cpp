#include "Environment.h"

// Environment::Environment(std::size_t birds_num):
//     _state(birds_num),
//     _v0(birds_num,0.1),
//     _capture_range{0.1},
//     _steering_angles(birds_num,M_PI/6)
//     {}

// Environment::Environment(std::size_t birds_num, double v0, double capture_range, double steering_angle):
//     _state(birds_num),
//     _v0(birds_num,v0),
//     _capture_range{capture_range},
//     _steering_angles(birds_num,steering_angle)
//     {}

Environment::Environment(std::size_t birds_num, double v_pursuer, double v_evader,
                         double friends_range, double capture_range, double steering_angle_pursuer, 
                         double steering_angle_evader, double pbc, double pursuer_vis_range,
                         std::pair<Angle,Angle> pursuer_vis_angle, double av_init_dist, double prey_repulsion):
    _state(birds_num, pbc,av_init_dist),
    _v0(birds_num,v_evader),
    _friends_range{friends_range},
    _capture_range{capture_range},
    _steering_angles(birds_num,steering_angle_evader),
    _pbc{pbc},
    _pursuer_vis_range{pursuer_vis_range},
    _pursuer_vis_angle{pursuer_vis_angle},
    _prey_repulsion{prey_repulsion}
    // _av_init_dist{av_init_dist}
    {
        _v0[0] = v_pursuer;
        _steering_angles[0] = steering_angle_pursuer;
    }


State Environment::dynamics(std::vector<Action> a, State& s, std::mt19937 &rng){

    //We introduce the confusion effect as noise in the predator vision.
    //Following Olson, Randal S., et al. "Predator confusion is sufficient to evolve 
    //swarming behaviour." Journal of The Royal Society Interface 10.85 (2013): 20130305.
    //We perturb the transitions from a state in which the predator sees 2+ preys
    
    // std::size_t ev_in_vis = 0;
    // auto birds = s.get_birds();
    // auto pred = birds[0];

    // //In our coordinate system, we need to rotate by -alpha to get what is in front of us
    // auto sin_alpha = -Angle::sin(pred.get_alpha());
    // auto cos_alpha = Angle::cos(pred.get_alpha());

    // for(std::size_t i=1; i<s.size(); ++i){
    //     if(relative_distance_squared(pred,birds[i],_pbc) < _pursuer_vis_range*_pursuer_vis_range){
    //         auto rel_angle = relative_angle(pred,birds[i],sin_alpha,cos_alpha,_pbc);
    //         if(rel_angle > _pursuer_vis_angle.first && rel_angle < _pursuer_vis_angle.second)
    //             ev_in_vis++;
    //     }
    // }

    //double correction = ev_in_vis == 0 ? 1 : (ev_in_vis+5.)/6;
    //std::cout << correction << std::endl;
    // auto correction = 1;
    // //Assigning different probabilities of taking the decided action
    // std::size_t pred_action = static_cast<std::size_t>(a[0]);
    // std::vector<double> w(3,0); //3 is the number of actions
    // for(std::size_t i=0;i<2;++i){
    //     if(i == pred_action)
    //         w[i] = 1./correction;
    //     else
    //         w[i] = (1-1./correction)/2;
    // }

    // std::initializer_list<double> weights{w[0],w[1],w[2]};
    // std::discrete_distribution<> random_act_dist(weights);

    // a[0] = static_cast<Action>(random_act_dist(rng));

    // switch (random_action)
    // {
    // case Action::left:

    //     s.update(_v0[0], _steering_angles[0], 0);
    //     break;
    
    // default:
    //     break;
    // }


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
    //auto friends_range = 0.5;

    auto birds = s.get_birds();
    auto predator = birds[0];
    auto pbc = s.get_pbc();

    //By default, we get a negative reward to pursuer and a positive one to evader
    r[0] = -1;
    for(std::size_t i=1; i<s.size(); ++i){
        r[i] = 1./num_of_preys;
    }

    for(std::size_t i=1; i<s.size(); ++i){
        if(relative_distance_squared(predator, birds[i],pbc) < _capture_range*_capture_range){
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
                if(relative_distance_squared(birds[i],birds[k],pbc) < _friends_range*_friends_range){
                    r[i] = -_prey_repulsion;
                    r[k] = -_prey_repulsion;
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
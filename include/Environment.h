#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include <vector>
#include <random>
#include "State.h"
#include "Reward.h"
#include "Action.h"
#include "Bird.h"
#include <initializer_list>

class Environment
{
    private:
        //Each bird has its own state. A vector of States defines the system
        State _state;
        std::vector<double> _v0;
        double _capture_range; 
        double _friends_range;
        std::vector<double> _steering_angles;
        double _pbc;
        double _pursuer_vis_range;
        std::pair<Angle,Angle> _pursuer_vis_angle;
        double _prey_repulsion;
        double _attraction_range;
        double _attraction_reward;

        //Update of single bird state
        void update(double velocity, double angle);

    public:

        //Constructors
        Environment(std::size_t birds_num, double v_pursuer, double v_evader, double friends_range, 
                    double capture_range, double steering_angle_pursuer, double steering_angle_evader, 
                    double pbc, double pursuer_vis_range, std::pair<Angle,Angle> pursuer_vis_angle,
                    double av_init_dist, double prey_repulsion, double attraction_range, double attraction_reward);

        //p(s'|a,s): One step dynamics. We are going to suppose to be in a deterministic settings, so s' = f(a,s)
        State dynamics(std::vector<Action> a, State& s);
        State random_evader(std::vector<Action> a, State& s);
        State& get_state(){return _state;}

        std::pair<Reward,bool> reward(State &s, double episode_length, int num_of_preys);
        void reset();
};



#endif
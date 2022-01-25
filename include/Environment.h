#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include <vector>
#include <random>
#include "State.h"
#include "Reward.h"
#include "Action.h"
#include "Bird.h"

class Environment
{
    private:
        //Each bird has its own state. A vector of States defines the system
        State _state;
        std::vector<double> _v0;
        double _capture_range; 
        double _friends_range;
        std::vector<double> _steering_angles;

        //Update of single bird state
        void update(double velocity, double angle);

    public:

        //Constructors
        // Environment(std::size_t birds_num);
        // Environment(std::size_t birds_num, double v0, double capture_range, double steering_angle);
        Environment(std::size_t birds_num, double v_pursuer, double v_evader, double friends_range, double capture_range, double steering_angle_pursuer, double steering_angle_evader, double pbc);

        //p(s'|a,s): One step dynamics. We are going to suppose to be in a deterministic settings, so s' = f(a,s)
        State dynamics(std::vector<Action> a, State& s);
        State random_evader(std::vector<Action> a, State& s);
        State& get_state(){return _state;}

        std::pair<Reward,bool> reward(State &s, double episode_length, int num_of_preys);
        void reset();
};



#endif
#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include <vector>
#include "State.h"
#include "Reward.h"
#include "Agent.h" //This is needed in order to have the actions


//TODO: the environment should know which agents are controlled from the external and which are not

class Environment
{
    private:
        //Each bird has its own state. A vector of States defines the system
        State _state;
        double _v0; //The velocity of the birds is a property of the environment.
        //We could define a different environment in which birds move faster
        double _capture_range; 
        double _steering_angle;
        //Update of single bird state
        void update(double velocity, double angle);

    public:

        //Constructors
        Environment(std::size_t birds_num);
        Environment(std::size_t birds_num, double v0, double capture_range, double steering_angle);

        //p(s'|a,s): One step dynamics. We are going to suppose to be in a deterministic settings, so s' = f(a,s)
        State dynamics(std::vector<Action> a, State& s);
        State random_evader(std::vector<Action> a, State& s);
        State& get_state(){return _state;}
        //Reward reward(State &s); //TODO
        std::pair<Reward,bool> reward(State &s);

        void reset();

};

#endif
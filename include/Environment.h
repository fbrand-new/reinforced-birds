#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include <vector>
#include "State.h"
#include "Agent.h" //This is needed in order to have the actions


//TODO: the environment should know which agents are controlled from the external and which are not

class Environment
{
    private:
        //Each bird has its own state. A vector of States defines the system
        State _state;
        double _v0; //The velocity of the birds is a property of the environment.
        //We could define a different environment in which birds move faster

        //Update of single bird state
        void update(double velocity, double angle);

    public:

        //Constructors
        Environment(unsigned int birds_num);

        //p(s'|a,s): One step dynamics. We are going to suppose to be in a deterministic settings, so s' = f(a,s)
        State dynamics(Action a, const State &s);
        State& get_state(){return _state;}
        double reward(Action &a, State &s); //TODO

};

#endif
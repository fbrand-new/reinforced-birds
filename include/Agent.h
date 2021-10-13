#ifndef __AGENT_H_
#define __AGENT_H_

#include "State.h"
#include "Observable.h"
#include "Action.h"
#include "Policy.h"
#include <vector>
#include <algorithm>

enum class Action{left, straight, right};

class Agent{

    private:
        std::size_t _id; //This is the id of the bird so that we can actually observe the others!
        Observable _o;
        Policy _p;

        double relative_distance(Bird &a, Bird &b);
        double relative_angle(Bird &a, Bird &b);

    public:

        //Getters
        std::size_t& get_id() {return _id;}
        Observable& get_observable() {return _o;}

        //Setters
        void set_id(std::size_t &&i) {_id = i;}

        // Observable function definition
        void obs(State &s);

        // Take an action according to a certain policy
        Action act(State &s);

        // Reward function definition 
        
};



#endif
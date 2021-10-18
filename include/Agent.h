#ifndef __AGENT_H_
#define __AGENT_H_

#include "State.h"
#include "Observable.h"
#include "Action.h"
#include "Policy.h"
#include <vector>
#include <memory>
#include <algorithm>

class Agent{

    private:
        std::size_t _id; //This is the id of the bird so that we can actually observe the others!
        Observable _o;
        //Policy * _p;
        std::unique_ptr<Policy> _p;
        double _vision_range;
        double _vision_angle;

    public:

        //Constructors
        Agent();
        Agent(Policy &p);
        Agent(double vision_range, double vision_angle);
        //Agent(std::size_t id, Policy &p);

        //Getters
        std::size_t& get_id() {return _id;}
        Observable& get_observable() {return _o;}
        auto& get_policy() {return _p;}

        //Setters
        void set_id(std::size_t &i) {_id = i;}

        template <typename T>
        void set_policy(Policy &&p) {_p = std::make_unique<T>(p);} 

        // Observable function definition
        Observable obs(State &s);
        // Take an action according to a certain policy and the current observation
        Action act(State &s, Observable &o);
        // Update policy given an observable and action
        void update_policy(double coeff, Observable &o, Action &a);

        //Utilities function
        static double relative_distance(const Bird &a, const Bird &b);
        static double relative_angle(const Bird &a, const Bird &b);
        
};



#endif
#ifndef __BASE_AGENT_H__
#define __BASE_AGENT_H__

#include "Angle.h"
#include "State.h"
#include "Timer.h"
#include "Observable.h"
#include "Action.h"
#include "Bird.h"
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

template <typename Observer, typename Policy, typename T>
class Agent{

    protected:
        std::size_t _id; //This is the id of the bird so that we can actually observe the others!
        std::unique_ptr<Policy> _p;
        std::unique_ptr<Observer> _o;       

    public:

        //Constructors
        Agent();
        Agent(Policy &&p, Observer &&o);
        Agent(Agent &&a) = default;

        //Getters
        std::size_t& get_id() {return _id;}
        auto& get_obs() {return _o;}
        auto& get_policy() {return _p;}

        //Setters
        void set_id(std::size_t &i) {_id = i;}
        void set_id(std::size_t &&i) {_id = std::move(i);}
        void set_policy(Policy &&p);

        Observable<T> obs(State &s) {return _o->obs(s); }

        // Take an action according to a certain policy and the current observation
        Action act(State &s, Observable<T> &o) {return _p->decide(o);}
        void update_policy(double coeff, Observable<T> &o, Action &a) {_p->update(coeff, o, a);}
};


template <typename Observer, typename Policy, typename T>
Agent<Observer,Policy,T>::Agent():
    _o{std::make_unique<Observer>()},
    _p{std::make_unique<Policy>()}
    {}

template <typename Observer, typename Policy, typename T>
Agent<Observer,Policy,T>::Agent(Policy &&p, Observer &&o):
    _p{std::make_unique<Policy>(std::move(p))},
    _o{std::make_unique<Observer>(std::move(o))}
    {}

template <typename Observer, typename Policy, typename T>
void Agent<Observer,Policy,T>::set_policy(Policy &&p){
    _p = std::make_unique<Policy>(std::move(p));
}

#endif
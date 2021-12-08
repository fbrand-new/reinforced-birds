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

/*
This is the base class for two different agents, in which two ways to observe the states are defined.
Please take a look at DirectedAgent.h and UndirectedAgent.h
*/

using int_pair = std::pair<std::size_t, std::size_t>;

enum class Obs_setting{foe_only, overwrite, both};

template <typename T, typename Policy>
class BaseAgent{

    protected:
        std::size_t _id; //This is the id of the bird so that we can actually observe the others!
        std::unique_ptr<Policy> _p;
        Observable<T> _o;
        double _vision_range;
        double _vision_angle;
        std::vector<double> _vision_sectors;

    public:

        //Constructors
        BaseAgent();
        BaseAgent(Policy &p);
        BaseAgent(double vision_range, double vision_angle);
        BaseAgent(std::size_t sector_num, std::size_t state_per_sector, double vision_range, double vision_angle);
        BaseAgent(BaseAgent &&a) = default;

        //Getters
        std::size_t& get_id() {return _id;}
        Observable<T>& get_obs() {return _o;}
        auto& get_policy() {return _p;}

        //Setters
        void set_id(std::size_t &i) {_id = i;}
        void set_id(std::size_t &&i) {_id = std::move(i);}
        void set_vision_range(double vision_range) {_vision_range = vision_range;}
        void set_vision_angle(double vision_angle) {_vision_angle = vision_angle;}
        void set_vision_sectors();
        void set_policy(Policy &&p);

        //Obsevable<T> obs(State &s, Obs_setting setting);

        // Take an action according to a certain policy and the current observation
        Action act(State &s, Observable<T> &o) {return _p->decide(o);}


        //void obs_opponent(std::size_t begin, std::size_t end, const Bird &me, const std::vector<Bird> &birds, double sin_alpha, double cos_alpha);
        //int obs_foe(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num);
        //int obs_brother(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, Obs_setting setting);
};


template <typename T, typename Policy>
BaseAgent<T,Policy>::BaseAgent():
    _o{5,3},
    _p{std::make_unique<Policy>()},
    _vision_range{15},
    _vision_angle{M_PI},
    _vision_sectors{std::vector<double>(_o.get_sectors_num()+1)}
    {}

template <typename T, typename Policy>
BaseAgent<T, Policy>::BaseAgent(std::size_t sector_num, std::size_t state_per_sector, double vision_range, double vision_angle):
    _o{sector_num, state_per_sector},
    //_p{std::make_unique<Policy>()},
    _vision_range{vision_range},
    _vision_angle{vision_angle},
    _vision_sectors{std::vector<double>(_o.get_sectors_num()+1)}
    {}

template <typename T, typename Policy>
BaseAgent<T, Policy>::BaseAgent(Policy &p):
    _p{std::make_unique<Policy>(p)}
    {}

template <typename T, typename Policy>
BaseAgent<T, Policy>::BaseAgent(double vision_range, double vision_angle):
    _p{std::make_unique<Policy>()},
    _vision_range{vision_range},
    _vision_angle{vision_angle}
    {}

template <typename T, typename Policy>
void BaseAgent<T, Policy>::set_vision_sectors(){

    auto vision_frac = _vision_angle/(_vision_sectors.size()-1);
    _vision_sectors[0] = -_vision_angle/2;
    for(std::size_t k=1; k<_vision_sectors.size(); ++k)
        _vision_sectors[k] = _vision_sectors[k-1] + vision_frac;

}

template <typename T, typename Policy>
void BaseAgent<T, Policy>::set_policy(Policy &&p){
    _p = std::make_unique<Policy>(std::move(p));
}

#endif
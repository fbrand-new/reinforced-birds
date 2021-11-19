#ifndef __AGENT_H_
#define __AGENT_H_

#include "State.h"
#include "Timer.h"
#include "Observable.h"
#include "Action.h"
#include "Policy.h"
#include <vector>
#include <memory>
#include <algorithm>

enum class Obs_setting{foe_only, overwrite, both};
enum class Obs_orientation{yes, no};

class Agent{

    private:
        std::size_t _id; //This is the id of the bird so that we can actually observe the others!
        Observable _o;
        std::unique_ptr<Policy> _p;
        double _vision_range;
        double _vision_angle;
        std::vector<double> _vision_sectors;
        std::pair<bool, double> out_of_scope(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha);


    public:

        //Constructors
        Agent();
        Agent(Policy &p);
        Agent(double vision_range, double vision_angle);
        Agent(std::size_t sector_num, std::size_t state_per_sector, double vision_range, double vision_angle);

        Agent(Agent &&a) = default;
        //

        //Getters
        std::size_t& get_id() {return _id;}
        Observable& get_obs() {return _o;}
        auto& get_policy() {return _p;}

        //Setters
        void set_id(std::size_t &i) {_id = i;}
        void set_vision_range(double vision_range) {_vision_range = vision_range;}
        void set_vision_angle(double vision_angle) {_vision_angle = vision_angle;}
        void set_vision_sectors();

        template <typename T>
        void set_policy(Policy &&p) {_p = std::make_unique<T>(p);} 

        // Observable function definition
        Observable obs(State &s, Obs_setting setting);
        Observable obs_opponent(State &s);
        Observable obs_overwrite(State &s);
        Observable obs_both(State &s);
        // Take an action according to a certain policy and the current observation
        Action act(State &s, Observable &o);
        // Update policy given an observable and action
        void update_policy(double coeff, Observable &o, Action &a);

        //Utilities function
        static double relative_distance(const Bird &a, const Bird &b);
        static double relative_angle(const double sin_alpha, const double cos_alpha, const Bird &a, const Bird &b);
        std::size_t obs_bird_overwrite(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, const std::vector<bool> &sector_occupied);
        int obs_bird_both(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, const std::vector<std::size_t> &sector_occupied);
        int obs_foe(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num);
        int obs_brother(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, Obs_setting setting);
        
        //std::size_t obs_bird(const Bird &me, const Bird &b, std::size_t sectors_num);
};



#endif
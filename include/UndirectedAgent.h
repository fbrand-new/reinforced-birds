#ifndef __UNDIRECTED_AGENT_H__
#define __UNDIRECTED_AGENT_H__

#include "BaseAgent.h"
#include "Boltzmann.h"

template <typename Policy>
class UndirectedAgent : public BaseAgent<UndirectedSector, Policy>{

    private:

    public:
        //Constructors
        UndirectedAgent();
        UndirectedAgent(Policy &p);
        UndirectedAgent(double vision_range, double vision_angle);
        UndirectedAgent(std::size_t sector_num, std::size_t state_per_sector, double vision_range, double vision_angle);
        UndirectedAgent(std::size_t sector_num, std::vector<size_t> states_per_sector, double vision_range, double vision_angle);
        UndirectedAgent(UndirectedAgent &&a);

        // Update policy given an observable and action
        //We use this only with Boltzmann policy: it will be an empty 
        //method except for Boltzmann policy in both subclasses
        void update_policy(double coeff, Observable<UndirectedSector> &o, Action &a);
        Observable<UndirectedSector> obs(State &s, Obs_setting setting);
        std::pair<bool,Angle> out_of_scope(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha);
        int obs_foe(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num);
        int obs_brother(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, Obs_setting setting);

        //Utilities functions
        
};

template <typename Policy>
UndirectedAgent<Policy>::UndirectedAgent(): 
    BaseAgent<UndirectedSector, Policy>()
    {}

template <typename Policy>
UndirectedAgent<Policy>::UndirectedAgent(Policy &p): 
    BaseAgent<UndirectedSector, Policy>(p)
    {}

template <typename Policy>
UndirectedAgent<Policy>::UndirectedAgent(double vision_range, double vision_angle):
    BaseAgent<UndirectedSector, Policy>(vision_range, vision_angle)
    {}

template <typename Policy>
UndirectedAgent<Policy>::UndirectedAgent(std::size_t sector_num, std::size_t state_per_sector, double vision_range, double vision_angle):
    BaseAgent<UndirectedSector, Policy>(sector_num, state_per_sector,vision_range,vision_angle)
    {}

template <typename Policy>
UndirectedAgent<Policy>::UndirectedAgent(std::size_t sector_num, std::vector<size_t> states_per_sector, double vision_range, double vision_angle):
    BaseAgent<UndirectedSector, Policy>(sector_num, states_per_sector,vision_range,vision_angle)
    {}

template <typename Policy>
UndirectedAgent<Policy>::UndirectedAgent(UndirectedAgent &&a):
    BaseAgent<UndirectedSector, Policy>(std::move(a))
    {}
    
template <typename Policy>
std::pair<bool, Angle> UndirectedAgent<Policy>::out_of_scope(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha){

    if (relative_distance_squared(me, b) > this->_vision_range*this->_vision_range)
        return std::make_pair(1, Angle()); //Since we are out of scope, Angles are just placeholders
    
    auto rel_angle = relative_angle(me, b, sin_alpha, cos_alpha);
  
    if(rel_angle.get() < this->_vision_sectors[0])
        return std::make_pair(1, Angle()); //Since we are out of scope, Angles are just placeholders

    return std::make_pair(0, rel_angle);
}

template <typename Policy>
int UndirectedAgent<Policy>::obs_foe(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num){

    auto info = out_of_scope(me, b, sin_alpha, cos_alpha);

    if(info.first == 1)
        return 0; //We are out of scope

    for(std::size_t i=1; i<=sectors_num; ++i){
        if(info.second.get() < this->_vision_sectors[i]){
            if(this->_o.get_sector(i-1) == UndirectedSector::foe){ //If we already have a foe in the sector
                return 0;
            } else {
                this->_o.set_sector(i-1,UndirectedSector::foe);
                return 1; //This is to update the count of birds that we've found
            }
        }
    }
    return 0;
}


template <typename Policy>
int UndirectedAgent<Policy>::obs_brother(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, Obs_setting setting){

    auto info = out_of_scope(me, b, sin_alpha, cos_alpha);

    if(info.first == 1)
        return 0; //We are indeed out of scope

    for(std::size_t i=1; i<sectors_num; ++i){
        if(info.second.get() < this->_vision_sectors[i]){
            if(this->_o.get_sector(i-1) == UndirectedSector::none){ //If the sector is empty fill it up
                this->_o.set_sector(i-1, UndirectedSector::brother);
                return 1;
            } 
            else if(setting == Obs_setting::both && this->_o.get_sector(i-1) == UndirectedSector::foe){ //If the sector is non empty AND you have the state both pursuer and evader
                this->_o.set_sector(i-1, UndirectedSector::both);
                return 1;
            }
        }
    }

    return 0;
}

template <typename Policy>
Observable<UndirectedSector> UndirectedAgent<Policy>::obs(State &s, Obs_setting setting){

    std::size_t sectors_num = this->_o.get_sectors_num();
    std::size_t pursuers_num = s.get_pursuer_num();
    std::size_t me_id = this->get_id();

    auto birds = s.get_birds();
    auto me = birds[me_id];

    //We want to rotate the system by -alpha(clockwise rotation)
    double sin_alpha = -Angle::sin(me.get_alpha()); 
    double cos_alpha = Angle::cos(me.get_alpha());

    std::size_t count = 0;

    //Reset observation
    for(std::size_t i=0; i<sectors_num; ++i)
        this->_o.set_sector(i,UndirectedSector::none);
 
    if(me.get_species() == Species::pursuer){
        for(std::size_t i=pursuers_num; count<sectors_num && i<birds.size(); ++i) //The for loop stops if we occupy all the sectors
            count += obs_foe(me, birds[i], sin_alpha, cos_alpha, sectors_num);
    } else {
        for(std::size_t i=0; count<sectors_num && i<pursuers_num; ++i)
            count += obs_foe(me, birds[i], sin_alpha, cos_alpha, sectors_num);
    }

    if(setting == Obs_setting::foe_only)
        return this->_o;

    count = 0;

    //TODO: it would be more efficient to perform the setting check here and have two different functions
    if(me.get_species() == Species::pursuer){
        for(std::size_t i=0; i<me_id && count<sectors_num; ++i)
            count += obs_brother(me, birds[i], sin_alpha, cos_alpha, sectors_num, setting);
        for(std::size_t i=me_id+1; i<pursuers_num && count<sectors_num; ++i)
            count += obs_brother(me, birds[i], sin_alpha, cos_alpha, sectors_num, setting);
    } else {
        for(std::size_t i=pursuers_num; i<me_id && count<sectors_num; ++i)
            count += obs_brother(me, birds[i], sin_alpha, cos_alpha, sectors_num, setting);
        for(std::size_t i=me_id+1; i<birds.size() && count<sectors_num; ++i)
            count += obs_brother(me, birds[i], sin_alpha, cos_alpha, sectors_num, setting);
    }   

    return this->_o;
}


#endif
#include "Agent.h"

Agent::Agent():
    _o{},
    _p{std::make_unique<Policy>()},
    _vision_range{10},
    _vision_angle{6/5*M_PI},
    _vision_sectors{std::vector<double>(_o.get_sectors_num()+1)}
    {}

Agent::Agent(Policy &p):
    _p{std::make_unique<Policy>(p)}
    {}

Agent::Agent(double vision_range, double vision_angle):
    _p{std::make_unique<Policy>()},
    _vision_range{vision_range},
    _vision_angle{vision_angle}
    {}

void Agent::set_vision_sectors(){

    auto vision_frac = _vision_angle/(_vision_sectors.size()-1);
    _vision_sectors[0] = -_vision_angle/2;
    for(std::size_t k=1; k<_vision_sectors.size(); ++k)
        _vision_sectors[k] = _vision_sectors[k-1] + vision_frac;

}

//TODO: We need to differentiate in between preys and predators OR add an additional state having both preys
//and predators
Observable Agent::obs(State &s){

    std::size_t sectors_num = this->_o.get_sectors_num();
    auto birds = s.get_birds();
    std::size_t pursuers_num = s.get_pursuer_num(); //This is also the index of the first prey!
    std::size_t me_id = this->get_id();
    auto me = birds[me_id];

    //
    std::vector<bool> sector_occupied(sectors_num);
    std::size_t num_sectors_occupied;
    
    //Resetting observation and defining the sectors
    for(std::size_t k=0; k<sectors_num; ++k){
        _o.empty_sector(k);
        sector_occupied[k] = 0;
    }

    //We should have a number of predators which occupies the first n_pred position of birds

    if(me.get_species() == Species::pursuer){

        //Predator sees preys
        for(std::size_t k=pursuers_num; k<birds.size(); ++k){
        
            if(num_sectors_occupied == sectors_num)
                return _o;

            auto temp_obs = obs_bird(me, birds[k], sectors_num, sector_occupied); //If non 0 is the occupied sector

            if(temp_obs==0){
                continue;
            } else {
                _o.non_empty_sector(temp_obs, 1); //1 indicates we have a prey
                sector_occupied[temp_obs] = 1;
                num_sectors_occupied +=1;
            }   
        }

        //Predator sees predators, up to itself
        for(std::size_t k=0; k<me_id; ++k){
        
            if(num_sectors_occupied == sectors_num)
                return _o;

            auto temp_obs = obs_bird(me, birds[k], sectors_num, sector_occupied); //If non 0 is the occupied sector

            if(temp_obs==0){
                continue;
            } else {
                _o.non_empty_sector(temp_obs, 2); //1 indicates we have a predator
                sector_occupied[temp_obs] = 1;
                num_sectors_occupied +=1;
            }   
        }

        //Predator sees predators, from itself to total number of pursuers
        for(std::size_t k=me_id+1; k<pursuers_num; ++k){
        
            if(num_sectors_occupied == sectors_num)
                return _o;

            auto temp_obs = obs_bird(me, birds[k], sectors_num, sector_occupied); //If non 0 is the occupied sector

            if(temp_obs==0){
                continue;
            } else {
                _o.non_empty_sector(temp_obs, 2); //1 indicates we have a predator
                sector_occupied[temp_obs] = 1;
                num_sectors_occupied +=1;
            }   
        }

    } else {

        //Prey sees predators
        for(std::size_t k=0; k<pursuers_num; ++k){
        
            if(num_sectors_occupied == sectors_num)
                return _o;

            auto temp_obs = obs_bird(me, birds[k], sectors_num, sector_occupied); //If non 0 is the occupied sector

            if(temp_obs==0){
                continue;
            } else {
                _o.non_empty_sector(temp_obs, 2); //1 indicates we have a predator
                sector_occupied[temp_obs] = 1;
                num_sectors_occupied +=1;
            }   
        }

        //Prey sees preys, up to itself
        for(std::size_t k=pursuers_num; k<me_id; ++k){
        
            if(num_sectors_occupied == sectors_num)
                return _o;

            auto temp_obs = obs_bird(me, birds[k], sectors_num, sector_occupied); //If non 0 is the occupied sector

            if(temp_obs==0){
                continue;
            } else {
                _o.non_empty_sector(temp_obs, 1); //1 indicates we have a prey on sight
                sector_occupied[temp_obs] = 1;
                num_sectors_occupied +=1;
            }   
        }

        //Predator sees predators, from itself to total number of pursuers
        for(std::size_t k=me_id+1; k<birds.size(); ++k){
        
            if(num_sectors_occupied == sectors_num)
                return _o;

            auto temp_obs = obs_bird(me, birds[k], sectors_num, sector_occupied); //If non 0 is the occupied sector

            if(temp_obs==0){
                continue;
            } else {
                _o.non_empty_sector(temp_obs, 1); //1 indicates we have a prey on sight
                sector_occupied[temp_obs] = 1;
                num_sectors_occupied +=1;
            }   
        }
    }

    return _o;
}

Action Agent::act(State &s, Observable &o){

    return _p->decide(o);
}

void Agent::update_policy(double coeff, Observable &o, Action &a){

    _p->update(coeff, o, a);
}

double Agent::relative_distance(const Bird &a, const Bird &b){

    auto x = a.get_x() - b.get_x();
    auto y = a.get_y() - b.get_y();
    return x*x + y*y;
    
}

double Agent::relative_angle(const Bird &a, const Bird &b){

    double alpha = a.get_alpha();

    //Centering the system on Bird a
    double x_rel = b.get_x() - a.get_x();
    double y_rel = b.get_y() - a.get_y();

    //Clockwise rotation of the system towards Bird a orientation
    double x_prime = cos(-alpha)*x_rel - sin(-alpha)*y_rel;
    double y_prime = sin(-alpha)*x_rel + cos(-alpha)*y_rel;
    
    return atan2(y_prime, x_prime);
}

std::size_t Agent::obs_bird(const Bird &me, const Bird &b, const std::size_t sectors_num, const std::vector<bool> &sector_occupied){
    
    if (relative_distance(me, b) > _vision_range*_vision_range)
        return 0; 
    
    auto rel_angle = relative_angle(me, b);

    if(rel_angle < _vision_sectors[0])
        return 0; //Out of vision cone

    for(std::size_t i=1; i<=sectors_num; ++i){
        if(rel_angle < _vision_sectors[i]){
            if(sector_occupied[i])
                return 0;
            else
                return i;
        }
    }

    return 0;
}



// std::size_t Agent::obs_bird(const Bird &me, const Bird &b, std::size_t sectors_num){
    
//     if (relative_distance(me, b) > _vision_range)
//         return 0; 
    
//     auto rel_angle = relative_angle(me, b);

//     if(rel_angle < _vision_sectors[0])
//         return 0; //Out of vision cone

//     for(std::size_t i=1; i<=sectors_num; ++i){
//         if(rel_angle < _vision_sectors[i])
//             return i;
//     }

//     return 0;
// }

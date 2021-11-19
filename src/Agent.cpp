#include "Agent.h"

Agent::Agent():
    _o{5,3},
    _p{std::make_unique<Policy>()},
    _vision_range{15},
    _vision_angle{M_PI},
    _vision_sectors{std::vector<double>(_o.get_sectors_num()+1)}
    {}

Agent::Agent(std::size_t sector_num, std::size_t state_per_sector, double vision_range, double vision_angle):
    _o{sector_num, state_per_sector},
    _p{std::make_unique<Policy>()},
    _vision_range{vision_range},
    _vision_angle{vision_angle},
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

Observable Agent::obs(State &s, Obs_setting setting){



    std::size_t sectors_num = this->_o.get_sectors_num();
    std::size_t pursuers_num = s.get_pursuer_num();
    std::size_t me_id = this->get_id();

    auto birds = s.get_birds();
    auto me = birds[me_id];

    double sin_alpha = sin(-me.get_alpha());
    double cos_alpha = cos(-me.get_alpha());

    std::size_t count = 0;

    //Reset observation
    for(std::size_t i=0; i<sectors_num; ++i)
        _o.set_sector(i,Bird_in_scope::none);
 
    if(me.get_species() == Species::pursuer){
        for(std::size_t i=pursuers_num; count<sectors_num && i<birds.size(); ++i) //The for loop stops if we occupy all the sectors
            count += obs_foe(me, birds[i], sin_alpha, cos_alpha, sectors_num);
    } else {
        for(std::size_t i=0; count<sectors_num && i<pursuers_num; ++i)
            count += obs_foe(me, birds[i], sin_alpha, cos_alpha, sectors_num);
    }

    if(setting == Obs_setting::foe_only)
        return _o;

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

    return _o;
}

// Observable Agent::obs_overwrite(State &s){

//     //Timer timethis;
//     std::size_t sectors_num = this->_o.get_sectors_num();
//     auto birds = s.get_birds();
//     std::size_t pursuers_num = s.get_pursuer_num(); //This is also the index of the first prey!
//     std::size_t me_id = this->get_id();
//     auto me = birds[me_id];

//     double sin_alpha = sin(-me.get_alpha());
//     double cos_alpha = cos(-me.get_alpha());

//     std::vector<bool> sector_occupied(sectors_num);
//     std::size_t num_sectors_occupied = 0;
    
//     //Resetting observation and defining the sectors
//     for(std::size_t k=0; k<sectors_num; ++k){
//         _o.empty_sector(k);
//         sector_occupied[k] = 0;
//     }

//     //We should have a number of predators which occupies the first n_pred position of birds

//     if(me.get_species() == Species::pursuer){

//         //Predator sees preys
//         for(std::size_t k=pursuers_num; k<birds.size(); ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_overwrite(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==0){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 1); //1 indicates we have a prey
//                 sector_occupied[temp_obs] = 1;
//                 num_sectors_occupied +=1;
//             }   
//         }

//         //Predator sees predators, up to itself
//         for(std::size_t k=0; k<me_id; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_overwrite(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==0){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 2); //1 indicates we have a predator
//                 sector_occupied[temp_obs] = 1;
//                 num_sectors_occupied +=1;
//             }   
//         }

//         //Predator sees predators, from itself to total number of pursuers
//         for(std::size_t k=me_id+1; k<pursuers_num; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_overwrite(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==0){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 2); //1 indicates we have a predator
//                 sector_occupied[temp_obs] = 1;
//                 num_sectors_occupied +=1;
//             }   
//         }

//     } else {

//         //Prey sees predators
//         for(std::size_t k=0; k<pursuers_num; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_overwrite(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==0){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 2); //1 indicates we have a predator
//                 sector_occupied[temp_obs] = 1;
//                 num_sectors_occupied +=1;
//             }   
//         }

//         //Prey sees preys, up to itself
//         for(std::size_t k=pursuers_num; k<me_id; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_overwrite(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==0){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 1); //1 indicates we have a prey on sight
//                 sector_occupied[temp_obs] = 1;
//                 num_sectors_occupied +=1;
//             }   
//         }

//         //Prey see preys, from itself to total number of pursuers
//         for(std::size_t k=me_id+1; k<birds.size(); ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_overwrite(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==0){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 1); //1 indicates we have a prey on sight
//                 sector_occupied[temp_obs] = 1;
//                 num_sectors_occupied +=1;
//             }   
//         }
//     }

//     return _o;
// }

// Observable Agent::obs_both(State &s){

//     // //Timer timethis;
//     std::size_t sectors_num = this->_o.get_sectors_num();
//     auto birds = s.get_birds();
//     std::size_t pursuers_num = s.get_pursuer_num(); //This is also the index of the first prey!
//     std::size_t me_id = this->get_id();
//     auto me = birds[me_id];

//     double sin_alpha = sin(-me.get_alpha());
//     double cos_alpha = cos(-me.get_alpha());

//     std::vector<std::size_t> sector_occupied(sectors_num);
//     std::size_t num_sectors_occupied = 0;
    
//     //Resetting observation and defining the sectors
//     for(std::size_t k=0; k<sectors_num; ++k){
//         _o.empty_sector(k);
//         sector_occupied[k] = 0;
//     }

//     // //We should have a number of predators which occupies the first n_pred position of birds
//     if(me.get_species() == Species::pursuer){

//         //Predator sees preys
//         for(std::size_t k=pursuers_num; k<birds.size(); ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 //return _o;
//                 break;

//             auto temp_obs = obs_bird_both(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==-1){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 1); //1 indicates we have a prey
//                 sector_occupied[temp_obs] = 1;
//                 num_sectors_occupied +=1;
//             }   
//         }

//         //Resetting occupancy of sectors
//         num_sectors_occupied = 0;

//         //Predator sees predators, up to itself
//         for(std::size_t k=0; k<me_id; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_both(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==-1){
//                 continue;
//             } else {
//                 if(sector_occupied[temp_obs] == 1){
//                     sector_occupied[temp_obs] = 3;
//                     _o.non_empty_sector(temp_obs, 3); //3 indicates we have both prey and predator
//                 } else {
//                     sector_occupied[temp_obs] = 2;
//                     _o.non_empty_sector(temp_obs, 2); //2 indicates we have a predator
//                 }
//                 num_sectors_occupied +=1;
//             }   
//         }

//         //Predator sees predators, from itself to total number of pursuers
//         for(std::size_t k=me_id+1; k<pursuers_num; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_both(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==-1){
//                 continue;
//             } else {
//                 if(sector_occupied[temp_obs] == 1){
//                     sector_occupied[temp_obs] = 3;
//                     _o.non_empty_sector(temp_obs, 3); //1 indicates we have both prey and predator
//                 } else {
//                     sector_occupied[temp_obs] = 2;
//                     _o.non_empty_sector(temp_obs, 2);
//                 }
//                 num_sectors_occupied +=1;
//             }    
//         }

//     } else {

//         //Prey sees predators
//         for(std::size_t k=0; k<pursuers_num; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 break;

//             auto temp_obs = obs_bird_both(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==-1){
//                 continue;
//             } else {
//                 _o.non_empty_sector(temp_obs, 2); //1 indicates we have a predator
//                 sector_occupied[temp_obs] = 2;
//                 num_sectors_occupied +=1;
//             }   
//         }

//         //Resetting occupancy of sectors
//         num_sectors_occupied = 0;

//         //Prey sees preys, up to itself
//         for(std::size_t k=pursuers_num; k<me_id; ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_both(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==-1){
//                 continue;
//             } else {
//                 if(sector_occupied[temp_obs] == 2){
//                     sector_occupied[temp_obs] = 3;
//                     _o.non_empty_sector(temp_obs, 3); //3 indicates we have both prey and predator
//                 } else {
//                     sector_occupied[temp_obs] = 1;
//                     _o.non_empty_sector(temp_obs, 1);
//                 }
//                 num_sectors_occupied +=1;
//             }  
//         }

//         //Prey see preys, from itself to total number of pursuers
//         for(std::size_t k=me_id+1; k<birds.size(); ++k){
        
//             if(num_sectors_occupied == sectors_num)
//                 return _o;

//             auto temp_obs = obs_bird_both(me, birds[k], sin_alpha, cos_alpha, sectors_num, sector_occupied); //If non 0 is the occupied sector

//             if(temp_obs==-1){
//                 continue;
//             } else {
//                 if(sector_occupied[temp_obs] == 2){
//                     sector_occupied[temp_obs] = 3;
//                     _o.non_empty_sector(temp_obs, 3); //3 indicates we have both prey and predator
//                 } else {
//                     sector_occupied[temp_obs] = 1;
//                     _o.non_empty_sector(temp_obs, 1);
//                 }
//                 num_sectors_occupied +=1;
//             }   
//         }
//     }

//     return _o;
// }

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

double Agent::relative_angle(const double sin_alpha, const double cos_alpha, const Bird &a, const Bird &b){

    //Centering the system on Bird a
    double x_rel = b.get_x() - a.get_x();
    double y_rel = b.get_y() - a.get_y();

    //Clockwise rotation of the system towards Bird a orientation
    double x_prime = cos_alpha*x_rel - sin_alpha*y_rel;
    double y_prime = sin_alpha*x_rel + cos_alpha*y_rel;
    
    return atan2(y_prime, x_prime);
}

// std::size_t Agent::obs_bird_overwrite(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, const std::vector<bool> &sector_occupied){
    
//     if (relative_distance(me, b) > _vision_range*_vision_range)
//         return 0; 
    
//     double rel_angle = relative_angle(sin_alpha, cos_alpha, me, b);
  
//     if(rel_angle < _vision_sectors[0])
//         return 0; //Out of vision cone

//     for(std::size_t i=1; i<=sectors_num; ++i){
//         if(rel_angle < _vision_sectors[i]){
//             if(sector_occupied[i])
//                 return 0;
//             else
//                 return i;
//         }
//     }

//     return 0;
// }

std::pair<bool, double> Agent::out_of_scope(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha){
    
    bool out_of_scope = 0;

    if (relative_distance(me, b) > _vision_range*_vision_range)
        out_of_scope = 1; //Too far away
    
    double rel_angle = relative_angle(sin_alpha, cos_alpha, me, b);
  
    if(rel_angle < _vision_sectors[0])
        out_of_scope = 1; //Out of vision cone

    return std::make_pair(out_of_scope, rel_angle);
}

int Agent::obs_foe(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num){

    auto info = out_of_scope(me, b, sin_alpha, cos_alpha);

    if(info.first == 1)
        return 0; //We are out of scope

    for(std::size_t i=1; i<=sectors_num; ++i){
        if(info.second < _vision_sectors[i]){
            if(_o.get_sector(i-1) == Bird_in_scope::foe){ //If we already have a foe in the sector
                return 0;
            } else {
                _o.set_sector(i-1,Bird_in_scope::foe);
                return 1; //This is to update the count of birds that we've found
            }
        }
    }

    return 0;
}

int Agent::obs_brother(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, Obs_setting setting){

    auto info = out_of_scope(me, b, sin_alpha, cos_alpha);

    if(info.first == 1)
        return 0; //We are indeed out of scope

    for(std::size_t i=1; i<sectors_num; ++i){
        if(info.second < _vision_sectors[i]){
            if(_o.get_sector(i-1) == Bird_in_scope::none){ //If the sector is empty fill it up
                _o.set_sector(i-1, Bird_in_scope::brother);
                return 1;
            } 
            else if(setting == Obs_setting::both && _o.get_sector(i-1) == Bird_in_scope::foe){ //If the sector is non empty AND you have the state both pursuer and evader
                _o.set_sector(i-1, Bird_in_scope::both);
                return 1;
            }
        }
    }

    return 0;
}


// int Agent::obs_bird_both(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha, const std::size_t sectors_num, const std::vector<std::size_t> &sector_occupied){
    
//     if (relative_distance(me, b) > _vision_range*_vision_range)
//         return -1; 
    
//     double rel_angle = relative_angle(sin_alpha, cos_alpha, me, b);
  
//     if(rel_angle < _vision_sectors[0])
//         return -1; //Out of vision cone

//     for(std::size_t i=1; i<=sectors_num; ++i){
//         if(rel_angle < _vision_sectors[i]){
//             if(sector_occupied[i-1] == 3) //The sector is occupied by both pursuer and evader
//                 return -1;
//             else if(sector_occupied[i-1] == 1 && b.get_species() == Species::evader) //Sector is occupied by a prey and b is a prey
//                 return -1;
//             else if(sector_occupied[i-1] == 2 && b.get_species() == Species::pursuer) //Sector is occupied by a predator and b is a predator
//                 return -1;
//             else
//                 return i-1;
//         }
//     }

//     return 0;
// }


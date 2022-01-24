#include "ClosestObserver.h"

// ClosestObserver::ClosestObserver(std::vector<Angle> meridians, double vis_range, std::size_t size, std::size_t me_id, double pbc):
//     _meridians{meridians},
//     _vision_range{vis_range},
//     _d{size},
//     _me_id{me_id},
//     _pbc{pbc}
//     {}

Observable<bool> ClosestObserver::obs(const State &s){

    //Obs dim is set a priori on the constructor of the observer
    auto obs_dim = _tiles_num;
    if(_me_id != 0)
        obs_dim *= 2;

    Observable<bool> o(obs_dim);
    
    //std::size_t pursuers_num = s.get_pursuer_num();
    auto birds = s.get_birds();
    auto me = birds[_me_id];

    //We want to rotate the system by -alpha(clockwise rotation)
    double sin_alpha = -Angle::sin(me.get_alpha()); 
    double cos_alpha = Angle::cos(me.get_alpha());

    auto predator_dist = 0;
    auto predator_angle = Angle();

    if(_me_id != 0){
        predator_dist = sqrt(relative_distance_squared(me,birds[0],_pbc));
        predator_angle = relative_angle(me,birds[0],sin_alpha,cos_alpha,_pbc);
    }

    auto closest_prey_dist = 2*_pbc;
    auto closest_prey_angle = Angle();

    for(std::size_t i=1; i<birds.size();++i){
        
        if(_me_id == i)
            continue;
        
        auto prey_dist = relative_distance_squared(me,birds[i],_pbc);

        if(prey_dist < closest_prey_dist*closest_prey_dist){
            auto prey_angle = relative_angle(me,birds[i],sin_alpha,cos_alpha,_pbc);
            if(prey_angle < _meridians[0] || prey_angle > _meridians[_meridians.size()-1]){
                continue;
            } else{
                closest_prey_dist = prey_dist;
                closest_prey_angle = prey_angle;
            }
        }
    }

    //We give the observer the responsibility to call set index before returning the observable
    auto idx = 0;

    if(closest_prey_dist < _vision_range*_vision_range){
        for(std::size_t i=1;i<_meridians.size();++i){
            if(closest_prey_angle < _meridians[i]){
                o.set(i-1,1);
                idx = i;
                break;
            }
        }
    }

    //If i am not the pursuer, locate the pursuer if its in the vision sector
    if(_me_id != 0){
        if(predator_dist < _vision_range*_vision_range){
            if(predator_angle > _meridians[0] && predator_angle < _meridians[_meridians.size()-1]){
                for(std::size_t i=1; i<_meridians.size();++i){
                    if(predator_angle < _meridians[i]){
                        o.set(_tiles_num+(i-1),1);
                        idx += (_tiles_num+1)*i;
                        break;
                    }
                }
            }
        }
    }

    o.set_index(idx);
    return o;

}

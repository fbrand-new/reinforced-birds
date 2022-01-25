#include "ClosestObsDirected.h"

Observable<bool> ClosestObsDirected::obs(const State &s){

    //Obs dim is set a priori on the constructor of the observer
    auto obs_dim = 2*_tiles_num; //Each tile can have prey in or prey out
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
    auto predator_heading = Angle();
    bool predator_heading_binary = false;

    if(_me_id != 0){
        predator_dist = sqrt(relative_distance_squared(me,birds[0],_pbc));
        predator_angle = relative_angle(me,birds[0],sin_alpha,cos_alpha,_pbc);
        predator_heading = birds[0].get_alpha() - me.get_alpha() - predator_angle;
        predator_heading_binary = 0 ? (predator_heading < M_PI_2 || predator_heading > -M_PI_2) : 1;

    }

    std::multimap<double,std::size_t> neighbours_dist;
    // std::vector<double> neighbours_evaders_dist(_neighbours,2*_pbc);
    // std::vector<Angle> neighbours_evaders_angle(_neighbours,0);
    // std::vector<std::size_t> neighbours_evaders_idx(_neighbours,0);
    // auto farthest_neigh = neighbours_evaders_dist.back();
    //auto closest_prey_angle = Angle();
    //std::size_t closest_prey_idx = 0;

    for(std::size_t i=1; i<birds.size();++i){
        
        if(_me_id == i)
            continue;
        
        auto prey_dist = relative_distance_squared(me,birds[i],_pbc);
        neighbours_dist.insert({prey_dist,i});
        // if(prey_dist < farthest_neigh*farthest_neigh){
        //     auto prey_angle = relative_angle(me,birds[i],sin_alpha,cos_alpha,_pbc);
        //     if(prey_angle < _meridians[0] || prey_angle > _meridians[_meridians.size()-1]){
        //         continue;
        //     } else{
        //         neighbours_evaders_dist.back() = prey_dist;
        //         //neighbours_evaders_dist[0] = prey_dist;
        //         closest_prey_angle = prey_angle;
        //         closest_prey_idx = i;
        //     }
        // }
    }

    // auto prey_heading = birds[closest_prey_idx].get_alpha() - me.get_alpha() - closest_prey_angle;
    // auto closest_prey_heading_binary = 0 ? (prey_heading < M_PI_2 || prey_heading > -M_PI_2) : 1;

    //We give the observer the responsibility to call set index before returning the observable
    std::size_t idx = 0;
    std::size_t count = 0;
    double farthest_ev = 0;


    for(auto &ev:neighbours_dist){
        for(std::size_t j=0; j <_parallels.size();++j){
            if(ev.first < _parallels[j]*_parallels[j]){
                auto ev_angle = relative_angle(me,birds[ev.second],sin_alpha,cos_alpha,_pbc);
                if(ev_angle > _meridians.front() && ev_angle < _meridians.back()){
                    auto prey_heading = birds[ev.second].get_alpha() - me.get_alpha() - ev_angle;
                    auto prey_heading_bin = 0 ? (prey_heading < M_PI_2 || prey_heading > -M_PI_2) : 1;
                    for(std::size_t i=1;i<_meridians.size();++i){
                        if(ev_angle < _meridians[i]){
                            if(prey_heading_bin == 0){
                                o.set(j*(_meridians.size()-1)+i-1,1);
                                idx += (count*2*_tiles_num+1)*(j*(_meridians.size()-1)+i); //Number of already possible states*where i am placed
                            } else {
                                o.set(_tiles_num+j*(_meridians.size()-1)+i-1,1);
                                idx += (count*2*_tiles_num+1)*(_tiles_num+j*(_meridians.size()-1)+i);
                            }
                            count++;
                            break;
                        }
                    }
                }
                break;
            }
        }
        if(count == _neighbours){
            farthest_ev = ev.first;
            break;
        }

    }


    //If i am not the pursuer, locate the pursuer if its in the vision sector
    //We now only see the predator if it see farther than the farthest evader
    //if(_me_id != 0 && farthest_ev > predator_dist){
    if(_me_id != 0){     
        for(std::size_t j=0; j <_parallels.size();++j){
            if(predator_dist < _parallels[j]*_parallels[j]){
                if(predator_angle > _meridians[0] && predator_angle < _meridians[_meridians.size()-1]){
                    for(std::size_t i=1; i<_meridians.size();++i){
                        if(predator_angle < _meridians[i]){
                            if(predator_heading_binary == 0){
                                o.set(2*_tiles_num+j*(_meridians.size()-1)+(i-1),1);
                                idx += (_neighbours*2*_tiles_num+1)*(i+j*(_meridians.size()-1));
                            } else {
                                o.set(3*_tiles_num+j*(_meridians.size()-1)+(i-1),1);
                                idx += (_neighbours*2*_tiles_num+1)*(_tiles_num+i+j*(_meridians.size()-1));
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    o.set_index(idx);
    return o;


}
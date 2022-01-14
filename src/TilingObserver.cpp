#include "TilingObserver.h"

TilingObserver::TilingObserver():
    _meridians{Angle(-3./4*M_PI), Angle(-M_PI/4),Angle(M_PI/4),Angle(3./4*M_PI)},
    _parallels{10},
    _vision_range{_parallels[_parallels.size()-1]},
    _tiles_num{(_meridians.size()-1)*_parallels.size()}
    {}

TilingObserver::TilingObserver(std::vector<Angle> meridians, std::vector<double> parallels, std::size_t me_id):
    _meridians{meridians},
    _parallels{parallels},
    _vision_range{_parallels[_parallels.size()-1]},
    _tiles_num{(_meridians.size()-1)*_parallels.size()},
    _me_id{me_id}
    {}

std::tuple<bool,double,Angle> TilingObserver::single_obs(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha){

    auto R = relative_distance_squared(me,b);
    if (R > this->_vision_range*this->_vision_range)
        return std::make_tuple(1,0,Angle()); //Since we are out of scope, Angles and radius are just placeholders
    
    auto rel_angle = relative_angle(me, b, sin_alpha, cos_alpha);
  
    if(rel_angle < this->_meridians[0] || rel_angle > this->_meridians[_meridians.size()-1])
        return std::make_tuple(1,0,Angle()); //Since we are out of scope, Angles are just placeholders

    return std::make_tuple(0,sqrt(R),rel_angle);
}

//Less flexible but more understandable than what we had so far.
//If we would like to try the other setting we should create other classes that do the same job
Observable<bool> TilingObserver::obs(State &s){

    std::size_t pursuers_num = s.get_pursuer_num();
    auto birds = s.get_birds();
    auto me = birds[_me_id];

    //We want to rotate the system by -alpha(clockwise rotation)
    double sin_alpha = -Angle::sin(me.get_alpha()); 
    double cos_alpha = Angle::cos(me.get_alpha());

    // Create new empty observable
    //Dimension of the observable corresponds to the number of tile times the amount of things
    //that we can see on each tile. For now it's just evaders and pursuers, so 2.
    //Moreover, we need to introduce an extra state representing the situation in which the observer
    //Is not seeing anyone else. 

    //TODO: change this 2 if we are going to consider the border
    Observable<bool> o(_tiles_num*2+1); 

    bool out_of_scope = false;
    double R = 0; //Distance between the observer and a single observation
    Angle theta = Angle(); //Angular distance between the observer and a single observation

    //Regardless of who we are, we want to observe evaders and pursuers
    std::size_t count = 0;

    for(std::size_t k=pursuers_num; k<birds.size(); ++k){
        
        if(k == _me_id)
            continue;
        
        std::tie(out_of_scope,R,theta) = single_obs(me, birds[k], sin_alpha, cos_alpha);

        if(out_of_scope)
            continue;
        
        for(auto i=0; i<_parallels.size(); ++i){
            if(R < _parallels[i]){
                //Right to left 
                for(auto j=1; j<_meridians.size(); ++j){
                    if(theta < _meridians[j]){
                        //Index of sector is given by row-wise stride over the tiling seen as a matrix
                        o.set(_tiles_num+i*(_meridians.size()-1)+(j-1),1);
                        count++;
                        break;
                    }
                }
                break;
            }
        }
    }

    for(std::size_t k=0; k<pursuers_num; ++k){
        
        if(k == _me_id)
            continue;
        
        std::tie(out_of_scope,R,theta) = single_obs(me, birds[k], sin_alpha, cos_alpha);

        if(out_of_scope)
            continue;
        
        for(auto i=0; i<_parallels.size(); ++i){
            if(R < _parallels[i]){
                //Right to left
                for(auto j=1; j<_meridians.size(); ++j){
                    if(theta < _meridians[j]){
                        o.set(i*(_meridians.size()-1)+(j-1),1);
                        count++;
                        break;
                    }
                }
                break;
            }
        }
    }

    //If we see nobody, then it means we are on the last state;
    if(count == 0)
        o.set(o.get_dim()-1,1);
    
    return o;

}
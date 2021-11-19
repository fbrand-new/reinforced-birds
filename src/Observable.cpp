#include "Observable.h"

Observable::Observable(): 
    _o{std::vector<Bird_in_scope>(5, Bird_in_scope::none)},
    _sectors{5},
    _max_val{1}
    {}

Observable::Observable(std::size_t sectors): 
    _o{std::vector<Bird_in_scope>(sectors, Bird_in_scope::none)},
    _sectors{sectors},
    _max_val{1}
    {}

Observable::Observable(std::size_t sectors, std::size_t max_val): 
    _o{std::vector<Bird_in_scope>(sectors, Bird_in_scope::none)},
    _sectors{sectors},
    _max_val{max_val}
    {}

std::vector<Bird_in_scope> Observable::get_obs(){
    return _o;
}

std::size_t Observable::get_sectors_num(){
    return _sectors;
}

std::size_t Observable::get_max_val(){
    return _max_val;
}

std::size_t Observable::index(){

    //0 to max_val**sectors -1 for predators
    //max_val**sectors to 2*max_val**sectors for preys
    std::size_t i = 0;

    for(std::size_t k=0; k<get_sectors_num(); k++){
        i+=static_cast<int>(_o[k])*pow(_max_val,k); //This transforms base _max_val+1 into a number
    }

    return i;
}

void Observable::set_sector(std::size_t i, Bird_in_scope b){
    _o[i] = b;
}

std::ostream& operator <<(std::ostream & os, Observable &o){

    auto obs = o.get_obs();

    for(std::size_t i=0; i<o.get_sectors_num()-1; ++i){
        os << static_cast<int>(obs[i]) << ",";
    }
    os << static_cast<int>(obs[o.get_sectors_num()-1]);

    //os << std::endl;
    return os;
}

std::ofstream& operator <<(std::ofstream & os, Observable &o){

    auto obs = o.get_obs();

    for(std::size_t i=0; i<o.get_sectors_num()-1; ++i){
        os << static_cast<int>(obs[i]) << ",";
    }
    os << static_cast<int>(obs[o.get_sectors_num()-1]);

    //os << std::endl;
    return os;
}
#include "Observable.h"

Observable::Observable(): 
    _o{std::vector<unsigned int>(5)},
    _sectors{5},
    _max_val{2}
    {}

Observable::Observable(std::size_t sectors): 
    _o{std::vector<unsigned int>(sectors)},
    _sectors{sectors},
    _max_val{1}
    {}

Observable::Observable(std::size_t sectors, std::size_t max_val): 
    _o{std::vector<unsigned int>(sectors)},
    _sectors{sectors},
    _max_val{max_val}
    {}

std::vector<unsigned int> Observable::get_obs(){
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
        i+=_o[k]*pow(_max_val+1,k); //This transforms base _max_val+1 into a number
    }

    return i;
}

void Observable::empty_sector(unsigned int i){
    _o[i] = 0;
}

void Observable::non_empty_sector(unsigned int i){
    _o[i] = 1;
}

void Observable::non_empty_sector(unsigned int i, bool pred){
    if(pred)
        _o[i] = _max_val;
    else
        _o[i] = 1;
}

void Observable::non_empty_sector(unsigned int i, observed o){
    _o[i] = static_cast<int>(o);
}

std::ostream& operator <<(std::ostream & os, Observable &o){

    auto obs = o.get_obs();

    for(std::size_t i=0; i<o.get_sectors_num(); ++i){
        os << obs[i];
    }

    os << std::endl;
    return os;
}
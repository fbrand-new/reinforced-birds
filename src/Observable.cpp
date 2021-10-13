#include "Observable.h"

Observable::Observable(): 
    _o{std::vector<unsigned int>(5)},
    _sectors{5},
    _max_val{1}
    {}

Observable::Observable(unsigned int sectors): 
    _o{std::make_shared<unsigned int []>(sectors)},
    _sectors{size},
    _max_val{1}
    {}

std::vector<unsigned int> get_obs(){
    return _o;
}

std::size_t Observable::get_sectors_num(){
    return _sectors;
}

void Observable::empty_sector(unsigned int i){
    _o[i] = 0;
}

void Observable::non_empty_sector(unsigned int i){
    _o[i] = 1;
}

std::ostream& operator <<(std::ostream & os, Observable &o){

    std::shared_ptr<unsigned int []> sectors = o.get_sectors();

    for(std::size_t i=0; i<o.get_sectors_num(); ++i){
        os << sectors[i];
    }

    os << std::endl;
    return os;
}
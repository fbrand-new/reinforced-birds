#ifndef __OBSERVABLE__
#define __OBSERVABLE__

#include <vector>
#include <math.h>
#include <fstream>
// #include "UndirectedObs.h"
// #include "DirectedObs.h"

//By defining the class as a templated class we cannot split declaration and implementation anymore
enum class DirectedSector{none, foe_in, foe_out, brother_in, brother_out, both};
enum class UndirectedSector{none, foe, brother, both};

template <typename T>
class Observable{

    private:
        std::vector<T> _o;
        std::size_t _sectors;
        std::vector<std::size_t> _states_per_sector;
        // std::size_t _states_per_sector;
        

    public:
        Observable():
            _o{std::vector<T>(5, T())},
            _sectors{5},
            _states_per_sector{std::vector<std::size_t>(5,1)}
            {}

        Observable(std::size_t sectors):
            _o{std::vector<T>(sectors, T())},
            _sectors{sectors},
            _states_per_sector{std::vector<std::size_t>(5,1)}
            {}

        Observable(std::size_t sectors, std::size_t states_per_sector):
            _o{std::vector<T>(sectors, T())},
            _sectors{sectors},
            _states_per_sector{std::vector<std::size_t>(sectors,states_per_sector)}
            {}

        Observable(std::size_t sectors, std::vector<std::size_t> &states_per_sector):
            _o{std::vector<T>(sectors, T())},
            _sectors{sectors},
            _states_per_sector{states_per_sector}
            {}

        //Getter
        std::vector<T> get_obs() {return _o;}
        std::size_t get_sectors_num() {return _sectors;}
        T get_sector(std::size_t i) { return _o[i];}
        
        //Setters
        void set_sector(std::size_t i, T b);
        std::size_t index();

        bool is_sector_empty(std::size_t i);
};

template <typename T, typename S>
S& operator <<(S & os, Observable<T> &o){

    auto obs = o.get_obs();

    for(std::size_t i=0; i<o.get_sectors_num()-1; ++i){
        os << static_cast<int>(obs[i]) << ",";
    }
    os << static_cast<int>(obs[o.get_sectors_num()-1]);

    return os;
}


template <typename T>
std::size_t Observable<T>::index(){

    //We treat a state as a number in base _states_per_sector
    //with each digit representing the state of a sector 
    //(e.g. 00100, meaning we only see a foe in the central sector)
    //and transform it into a number in base 10 for indexing.
    
    //Each potential observed state MUST have a to_int method

    std::size_t i = 0;
    std::size_t base = 1;

    for(std::size_t k=0; k<get_sectors_num(); k++){
        //i += (_o[k].to_int())*pow(_states_per_sector,k);
        i += static_cast<int>(_o[k])*base;
        base *= _states_per_sector[k];
    }

    return i;
}

template <typename T>
bool Observable<T>::is_sector_empty(std::size_t i){
    if(get_sector(i) == T::none)
        return true;
    else
        return false;
}

template<typename T>
void Observable<T>::set_sector(std::size_t i, T b){
    if(static_cast<std::size_t>(b) < _states_per_sector[i])
        _o[i] = b;
    else
        _o[i] = T::none;
}

#endif
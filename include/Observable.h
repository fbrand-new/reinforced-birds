#ifndef __OBSERVABLE__
#define __OBSERVABLE__

#include <vector>
#include <math.h>
#include <fstream>

template <typename T>
class Observable{

    private:
        std::vector<T> _o; 
        std::size_t _d; //Dimension of the observable space
        std::size_t _index; //TODO: probably set me in the constructor

    public:
        // Observable():
            // _o{std::vector<T>(5)},
            // _sectors_num{5}
            // {}

        Observable(std::size_t size):
            _o{std::vector<T>(size)},
            _d{size},
            _index{0}
            {}

        Observable(std::size_t size, std::size_t index):
            _o{std::vector<T>(size)},
            _d{size},
            _index{index}
            {}

        //Getter
        auto& get_obs() {return _o;}
        std::size_t get_dim() {return _d;}
        T operator[](std::size_t i) {return _o[i];}
        const T operator[](std::size_t i) const noexcept {return _o[i];}
        auto& index() const {return _index;}
        //auto& get_sector(std::size_t i) { return _o[i];}

        
        //Setters
        void set(std::size_t i, T s);
        void set_index(std::size_t idx) {_index=idx;}
        // std::size_t index();

        //bool is_sector_empty(std::size_t i);
};

// template<typename T>
// void Observable<T>::set(std::size_t i, T s){
//     _o[i] = s;
//     //set_index();
// }

// template<typename T>
// void Observable<T>::set_index(){

// }


template <typename T, typename S>
S& operator <<(S & os, Observable<T> &o){

    auto obs = o.get_obs();

    for(std::size_t i=0; i<o.get_dim()-1; ++i){
        os << obs[i] << ",";
    }
    os << obs[o.get_dim()-1];

    return os;
}


// template <typename T>
// std::size_t Observable<T>::index(){

//     //We treat a state as a number in base _states_per_sector
//     //with each digit representing the state of a sector 
//     //(e.g. 00100, meaning we only see a foe in the central sector)
//     //and transform it into a number in base 10 for indexing.
    
//     //Each potential observed state MUST have a to_int method

//     std::size_t i = 0;
//     std::size_t base = 1;

//     for(std::size_t k=0; k<get_sectors_num(); k++){
//         //i += (_o[k].to_int())*pow(_states_per_sector,k);
//         i += static_cast<int>(_o[k])*base;
//         base *= _states_per_sector[k];
//     }

//     return i;
// }




#endif
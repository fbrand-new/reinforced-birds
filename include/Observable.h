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
        std::size_t _index;

    public:
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

        
        //Setters
        void set(std::size_t i, T s);
        void set_index(std::size_t idx) {_index=idx;}
};


template <typename T, typename S>
S& operator <<(S & os, Observable<T> &o){

    auto obs = o.get_obs();

    for(std::size_t i=0; i<o.get_dim()-1; ++i){
        os << obs[i] << ",";
    }
    os << obs[o.get_dim()-1];

    return os;
}

#endif
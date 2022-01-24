#include "Observable.h"

// template<bool>
//It is better to define each case in its proper way?

// template<>
// void Observable<bool>::set_index(){
//     std::size_t idx = 0;
//     for(std::size_t i=0; i<_o.size(); ++i)
//         idx += _o[i]*pow(2,i);
//     _index = idx;
// }

template<>
void Observable<bool>::set(std::size_t i, bool s){
    _o[i] = s;
}


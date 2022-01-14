#include "V.h"

V::V(std::size_t size):
    _w(size)
    {}

double& V::operator[](std::size_t i){
    return _w[i];
}

// double trace(std::vector<double> a){
    
//     double res = 0;
//     for(auto &i:a)
//         res += i;

//     return res;
// }

// double trace(const V &v){
//     return trace(v.get());
// }
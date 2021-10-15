#include "V.h"

V::V(std::size_t size):
    _w{size}
    {}

double& V::operator[](Observable &o){
    return _w[o.index()];
}

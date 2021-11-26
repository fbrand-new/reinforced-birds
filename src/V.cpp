#include "V.h"

V::V(std::size_t size):
    _w(size)
    {}

double& V::operator[](std::size_t i){
    return _w[i];
}